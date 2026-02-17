#include <iostream>
#include <iomanip>
#include <string>
#include <any>
#include <typeinfo>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <functional>
#include <boost/variant.hpp>
#include "history_manager.hpp"
#include "graph_snapshot.hpp"
#include "graph_db.hpp"
#include "query_ctx.hpp"
#include "qop_as_of_scan.hpp"
#include "qop_timeline_scan.hpp"
#include "qop_temporal_expand.hpp"
#include "qop_time_slice_expand.hpp"
#include "qresult.hpp"

static std::unordered_map<uint64_t, uint64_t> tt_mapping;
static uint64_t next_tt_id = 1;
static std::string fmt_time(uint64_t t) { return (t == MAX_TIME) ? "MAX" : std::to_string(t); }

// Format Transaction Time with consistent IDs
static std::string fmt_tt(uint64_t t) {
    if (t == MAX_TIME || t == INF || t == UINT64_MAX) return "MAX";
    if (t == 0) return "T0";
    if (tt_mapping.find(t) == tt_mapping.end()) tt_mapping[t] = next_tt_id++;
    return "T" + std::to_string(tt_mapping[t]);
}

// PROPERTY HELPERS
static void print_props(const properties_t& props) {
    if (props.empty()) { std::cout << "{}"; return; }
    std::cout << "{"; bool first = true;
    for (const auto& [key, val] : props) {
        if (!first) std::cout << ", ";
        if (val.type() == typeid(std::string)) std::cout << key << ": \"" << std::any_cast<std::string>(val) << "\"";
        else if (val.type() == typeid(uint64_t)) std::cout << key << ": " << std::any_cast<uint64_t>(val);
        else if (val.type() == typeid(int)) std::cout << key << ": " << std::any_cast<int>(val);
        else std::cout << key << ": <val>";
        first = false;
    }
    std::cout << "}";
}

// Compare two property sets for equality
static bool props_equal(const properties_t& a, const properties_t& b) {
    if (a.size() != b.size()) return false;
    for (const auto& [key, valA] : a) {
        auto it = b.find(key);
        if (it == b.end()) return false;
        if (valA.type() == typeid(std::string)) {
            if (std::any_cast<std::string>(valA) != std::any_cast<std::string>(it->second)) return false;
        } else if (valA.type() == typeid(uint64_t)) {
            if (std::any_cast<uint64_t>(valA) != std::any_cast<uint64_t>(it->second)) return false;
        }
    }
    return true;
}

// Print differences between two property sets
static void print_prop_diff(const properties_t& a, const properties_t& b) {
    std::vector<std::string> keys;
    for (const auto& [k, _] : a) keys.push_back(k);
    for (const auto& [k, _] : b) keys.push_back(k);
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    auto render = [](const std::any& v) -> std::string {
        if (v.type() == typeid(std::string)) return "\"" + std::any_cast<std::string>(v) + "\"";
        if (v.type() == typeid(uint64_t)) return std::to_string(std::any_cast<uint64_t>(v));
        return "<?>";
    };
    for (const auto& k : keys) {
        auto ia = a.find(k), ib = b.find(k);
        if (ia != a.end() && ib != b.end()) {
            if (render(ia->second) != render(ib->second))
                std::cout << "      " << k << ": " << render(ia->second) << " -> " << render(ib->second) << "\n";
        } else std::cout << "      " << k << ": " << (ia != a.end() ? render(ia->second) : "<none>") << " -> " << (ib != b.end() ? render(ib->second) : "<none>") << "\n";
    }
}

// OPERATORS
struct TupleSource : public qop { 
    std::vector<uint64_t> ids_;
    TupleSource(const std::unordered_set<uint64_t>& ids) : ids_(ids.begin(), ids.end()) { type_ = qop_type::scan; } 
    void start(query_ctx &ctx) override {
        for (auto id : ids_) {
            qr_tuple t; t.push_back(id);
            if (consume_) consume(ctx, t);
        }
        qop::default_finish(ctx);
    }
    void accept(qop_visitor &vis) override { if (has_subscriber()) subscriber_->accept(vis); }
    void dump(std::ostream &os) const override { os << "TupleSource"; }
};

struct TemporalPrinter : public qop { 
    static std::mutex out_mtx;
    TemporalPrinter() { type_ = qop_type::printer; }

    void process(query_ctx &ctx, const qr_tuple &v) {
        std::lock_guard<std::mutex> lock(out_mtx);
        if (v.empty()) return;

        if (v.size() >= 2 && v[1].type() == typeid(rship_description)) {
            const auto& n = boost::get<node_description>(v[0]);
            const auto& r = boost::get<rship_description>(v[1]);

            std::cout << "  [Expansion Result]\n";
            std::cout << "    Link: [REL " << r.logical_id << "] Type: " << r.label << " ";
            print_props(r.properties);
            std::cout << " (VT: " << r.vt_start << "-" << fmt_time(r.vt_end) << ")\n";

            std::cout << "    Target: [NODE " << n.logical_id << "] ";
            print_props(n.properties);
            std::cout << " (VT: " << n.vt_start << "-" << fmt_time(n.vt_end) << ")\n\n";
        }
        else {
            for (const auto& res : v) {
                if (res.which() == node_descr_type) {
                    const auto& d = boost::get<node_description>(res);
                    std::cout << "  [NODE " << d.logical_id << "] "; print_props(d.properties);
                    std::cout << " VT: [" << d.vt_start << "-" << fmt_time(d.vt_end) << ")\n";
                }
            }
        }
    }

    void finish(query_ctx &ctx) { std::cout << "  Operator Stream Finished \n"; }
    void accept(qop_visitor &vis) override { if (has_subscriber()) subscriber_->accept(vis); }
    void dump(std::ostream &os) const override { os << "TemporalPrinter"; }
};
std::mutex TemporalPrinter::out_mtx;

// WRAPPERS
void execute_bulk_as_of(std::shared_ptr<graph_db> db, const std::unordered_set<uint64_t>& lids, uint64_t vt, bool is_node) {
    if (lids.empty()) return;
    query_ctx ctx; ctx.gdb_ = db;
    auto source_op = std::make_shared<TupleSource>(lids);
    auto scan_op = std::make_shared<poseidon::qop_as_of_scan>(vt, is_node);
    auto print_op = std::make_shared<TemporalPrinter>();

    source_op->connect(scan_op,
        std::bind(&poseidon::qop_as_of_scan::process, scan_op.get(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&poseidon::qop_as_of_scan::finish, scan_op.get(), std::placeholders::_1));

    scan_op->connect(print_op,
        std::bind(&TemporalPrinter::process, print_op.get(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&TemporalPrinter::finish, print_op.get(), std::placeholders::_1));

    source_op->start(ctx);
}

void execute_bulk_timeline(std::shared_ptr<graph_db> db, const std::unordered_set<uint64_t>& lids, uint64_t start, uint64_t end, bool is_node) {
    if (lids.empty()) return;
    query_ctx ctx; ctx.gdb_ = db;
    auto source_op = std::make_shared<TupleSource>(lids);
    auto scan_op   = std::make_shared<poseidon::qop_timeline_scan>(start, end, is_node);
    auto print_op  = std::make_shared<TemporalPrinter>();

    source_op->connect(scan_op,
        std::bind(&poseidon::qop_timeline_scan::process, scan_op.get(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&poseidon::qop_timeline_scan::finish, scan_op.get(), std::placeholders::_1));

    scan_op->connect(print_op,
        std::bind(&TemporalPrinter::process, print_op.get(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&TemporalPrinter::finish, print_op.get(), std::placeholders::_1));

    source_op->start(ctx);
}

void execute_temporal_expansion(std::shared_ptr<graph_db> db, uint64_t start_lid, uint64_t vt, bool is_out, std::string label = "") {
    query_ctx ctx; ctx.gdb_ = db;
    auto source_op = std::make_shared<TupleSource>(std::unordered_set<uint64_t>{start_lid});
    auto scan_op = std::make_shared<poseidon::qop_as_of_scan>(vt, true);
    auto expand_op = std::make_shared<poseidon::qop_temporal_expand>(vt, is_out, label);
    auto print_op = std::make_shared<TemporalPrinter>();

    source_op->connect(scan_op,
        std::bind(&poseidon::qop_as_of_scan::process, scan_op.get(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&poseidon::qop_as_of_scan::finish, scan_op.get(), std::placeholders::_1));

    scan_op->connect(expand_op,
        std::bind(&poseidon::qop_temporal_expand::process, expand_op.get(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&poseidon::qop_temporal_expand::finish, expand_op.get(), std::placeholders::_1));

    expand_op->connect(print_op,
        std::bind(&TemporalPrinter::process, print_op.get(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&TemporalPrinter::finish, print_op.get(), std::placeholders::_1));

    source_op->start(ctx);
}

void execute_time_slice_expansion(std::shared_ptr<graph_db> db, uint64_t start_lid, uint64_t t_start, uint64_t t_end, bool is_out = true) {
    query_ctx ctx; ctx.gdb_ = db;
    std::cout << "\n GENERIC TIME-SLICE EXPANSION (Start=" << start_lid << ", Interval=[" << t_start << ", " << t_end << "]) ---\n";
    auto source_op = std::make_shared<TupleSource>(std::unordered_set<uint64_t>{start_lid});
    auto scan_op   = std::make_shared<poseidon::qop_as_of_scan>(t_start, true); 
    auto slice_op  = std::make_shared<poseidon::qop_time_slice_expand>(t_start, t_end, is_out, "");
    auto print_op  = std::make_shared<TemporalPrinter>();

    source_op->connect(scan_op,
        std::bind(&poseidon::qop_as_of_scan::process, scan_op.get(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&poseidon::qop_as_of_scan::finish, scan_op.get(), std::placeholders::_1));

    scan_op->connect(slice_op,
        std::bind(&poseidon::qop_time_slice_expand::process, slice_op.get(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&poseidon::qop_time_slice_expand::finish, slice_op.get(), std::placeholders::_1));

    slice_op->connect(print_op,
        std::bind(&TemporalPrinter::process, print_op.get(), std::placeholders::_1, std::placeholders::_2),
        std::bind(&TemporalPrinter::finish, print_op.get(), std::placeholders::_1));

    source_op->start(ctx);
}

// VERIFICATION HELPERS 
void print_snapshot_automated(graph_db& db, uint64_t query_vt) {
    std::cout << "\n=== SNAPSHOT at ValidTime: " << query_vt << " ===\n";
    std::cout << std::setw(12) << "LogicalID" << " | " << std::setw(30) << "Properties" << " | ValidTime | TransactionTime\n";
    std::cout << std::string(90, '-') << "\n";
    for (auto lid : db.get_active_node_lids()) {
        if (auto nid = db.try_get_node_at_vt(lid, query_vt)) {
            auto desc = db.get_node_description(*nid);
            std::cout << std::setw(12) << lid << " | "; print_props(desc.properties);
            std::cout << "\n" << std::setw(12) << "" << " | VT=[" << desc.vt_start << ", " << fmt_time(desc.vt_end) << ") TT=[" << fmt_tt(desc.tt_start) << ", " << fmt_tt(desc.tt_end) << ")\n";
        }
    }
    for (auto lid : db.get_active_rship_lids()) {
        if (auto rid = db.try_get_rship_at_vt(lid, query_vt)) {
            auto desc = db.get_rship_description(*rid);
            std::cout << std::setw(12) << lid << " | "; print_props(desc.properties);
            std::cout << "\n" << std::setw(12) << "" << " | VT=[" << desc.vt_start << ", " << fmt_time(desc.vt_end) << ") TT=[" << fmt_tt(desc.tt_start) << ", " << fmt_tt(desc.tt_end) << ")\n";
        }
    }
}

void print_diff_automated(graph_db& db, uint64_t t1, uint64_t t2) {
    std::cout << "\nDIFF (ValidTime: " << t1 << " vs " << t2 << ") ===\n";
    for (auto lid : db.get_active_node_lids()) {
        auto n1 = db.try_get_node_at_vt(lid, t1), n2 = db.try_get_node_at_vt(lid, t2);
        if (n1 && n2) {
            auto d1 = db.get_node_description(*n1), d2 = db.get_node_description(*n2);
            if (d1.id != d2.id) {
                std::cout << "  [UPDATE] Node " << lid << "\n";
                if (!props_equal(d1.properties, d2.properties)) print_prop_diff(d1.properties, d2.properties);
            }
        }
    }
}

int main() {
    auto db_ptr = std::make_shared<graph_db>("social_network_temporal", ".", 5000);
    graph_db& db = *db_ptr;

    node::id_t a1, b1, c1;
    node::id_t a2, b2, a3;

    db.begin_transaction();
    a1 = db.add_node("Person", {{"name", std::string("Alice")}, {"age", uint64_t(28)}, {"role", std::string("Software Engineer")}}, 2010, MAX_TIME, 1);
    b1 = db.add_node("Person", {{"name", std::string("Bob")}, {"age", uint64_t(30)}, {"role", std::string("Product Manager")}}, 2010, MAX_TIME, 2);
    db.add_relationship(a1, b1, "KNOWS", {{"since", uint64_t(2010)}}, 2010, MAX_TIME, 100);
    db.commit_transaction();

    db.begin_transaction(); // T2: Charlie joins
    c1 = db.add_node("Person", {{"name", std::string("Charlie")}, {"age", uint64_t(26)}, {"role", std::string("Data Scientist")}}, 2013, MAX_TIME, 3);
    db.add_relationship(b1, c1, "KNOWS", {{"since", uint64_t(2013)}}, 2013, MAX_TIME, 101);
    db.commit_transaction();

    db.begin_transaction(); // T3: Alice Promotion
    a2 = db.update_node_create_version(a1, {{"age", uint64_t(33)}, {"role", std::string("Senior Software Engineer")}}, 2015);
    db.add_relationship(a2, c1, "WORKS_WITH", {{"project", std::string("AI Platform")}}, 2015, MAX_TIME, 102);
    db.commit_transaction();

    db.begin_transaction(); // T4: Bob Role Change
    b2 = db.update_node_create_version(b1, {{"age", uint64_t(38)}, {"role", std::string("Engineering Manager")}}, 2018);
    db.commit_transaction();

    db.begin_transaction(); // T5: Alice Tech Lead
    a3 = db.update_node_create_version(a2, {{"age", uint64_t(38)}, {"role", std::string("Tech Lead")}}, 2020);
    db.commit_transaction();

    db.begin_transaction(); // T6: Close relationship
    auto rel_id = db.try_get_rship_at_vt(100, 2018).value();
    db.close_relationship_at_vt(rel_id, 2019);
    db.commit_transaction();

    db.begin_transaction(); // T7: Relationship update
    auto rel_102 = db.try_get_rship_at_vt(102, 2019).value();
    db.update_relationship_create_version(rel_102, {{"ended", std::string("Project completed")}}, 2020);
    db.commit_transaction();

    // --- STRESS TEST: Trigger Checkpoint for Alice (Interval=8) ---
    std::cout << "\n[STRESS] Creating 10 versions for Alice to trigger Checkpoint Jump logic...\n";
    node::id_t stress_id = a3;
    for(int i=0; i < 10; ++i) {
        db.begin_transaction();
        stress_id = db.update_node_create_version(stress_id, {{"v", uint64_t(i)}}, 2025 + i);
        db.commit_transaction();
    }

    std::cout << "\n=== TEMPORAL DB Verification ===\n";

    db.begin_transaction();
    const auto& node_lids = db.get_active_node_lids();
    const auto& rship_lids = db.get_active_rship_lids();
    for (uint64_t year : {2011, 2014, 2017, 2021}) {
        std::cout << "\n GENERIC SNAPSHOT PIPELINE (VT=" << year << ") ---\n";
        execute_bulk_as_of(db_ptr, node_lids, year, true);
        execute_bulk_as_of(db_ptr, rship_lids, year, false);
    }
    std::cout << "\n GENERIC TIMELINE PIPELINES (All History) ---\n";
    execute_bulk_timeline(db_ptr, node_lids, 2010, 2035, true);
    execute_bulk_timeline(db_ptr, rship_lids, 2010, 2035, false);
    db.commit_transaction();

    // --- REIMPLEMENTED SECTION: GENERIC EXPANSION PIPELINES ---
    std::cout << "\n GENERIC EXPANSION PIPELINES (Optimized Checkpoint Jump Verification) ---\n";
    db.begin_transaction();
    for (uint64_t year : {2011, 2014, 2021, 2030}) { // Year 2030 triggers the Checkpoint Jump
        std::cout << "\n-- Expansions at VT=" << year << " --\n";
        for (auto lid : node_lids) {
            execute_temporal_expansion(db_ptr, lid, year, true, "");
        }
    }
    db.commit_transaction();

    // --- REIMPLEMENTED SECTION: GENERIC TIME-SLICE TEST ---
    std::cout << "\n GENERIC TIME-SLICE TEST (Alice LID 1 - Optimized Coverage) ---\n";
    db.begin_transaction();
    
    std::cout << "Scenario: Neighbors active for full duration 2011 to 2012 (Initial Fallback Path)\n";
    execute_time_slice_expansion(db_ptr, 1, 2011, 2012, true);

    std::cout << "Scenario: Neighbors active for full duration 2016 to 2018 (Intermediate Coverage)\n";
    execute_time_slice_expansion(db_ptr, 1, 2016, 2018, true);

    std::cout << "Scenario: Interval [2018, 2021] (Link deletion check)\n";
    execute_time_slice_expansion(db_ptr, 1, 2018, 2021, true);

    std::cout << "Scenario: Post-Checkpoint Slice [2030, 2032] (Verifying jump logic coverage)\n";
    execute_time_slice_expansion(db_ptr, 1, 2030, 2032, true);

    db.commit_transaction();


    // MANUAL VERIFICATION
    db.begin_transaction();
    std::cout << "\n-SNAPSHOTS (Manual state verification)-\n";
    for (uint64_t t : {2011, 2014, 2017, 2021}) print_snapshot_automated(db, t);

    std::cout << "\n-HISTORICAL STORAGE (Diff queries)-\n";
    print_diff_automated(db, 2014, 2017);
    print_diff_automated(db, 2017, 2021);

    std::cout << "\n-ANCHOR SNAPSHOT TEST-\n";
    std::unordered_set<uint64_t> change_times;
    for (auto lid : node_lids) {
        for (auto nid : db.get_node_versions_in_vt(lid, 0, MAX_TIME)) {
            auto d = db.get_node_description(nid);
            change_times.insert(d.vt_start); if (d.vt_end != MAX_TIME) change_times.insert(d.vt_end);
        }
    }
    for (auto lid : rship_lids) {
        for (auto rid : db.get_rship_versions_in_vt(lid, 0, MAX_TIME)) {
            auto d = db.get_rship_description(rid);
            change_times.insert(d.vt_start); if (d.vt_end != MAX_TIME) change_times.insert(d.vt_end);
        }
    }
    std::vector<uint64_t> t_list(change_times.begin(), change_times.end()); std::sort(t_list.begin(), t_list.end());
    for (uint64_t ts : t_list) {
        GraphSnapshot snap = db.get_graph_as_of(ts);
        std::cout << "Snapshot at t=" << ts << " -> nodes=" << snap.nodes.size() << ", rels=" << snap.relationships.size() << "\n";
        std::cout << "  Nodes: "; for (auto& n : snap.nodes) std::cout << n.logical_id << " ";
        std::cout << "\n  Rels:  "; for (auto& r : snap.relationships) std::cout << r.logical_id << " ";
        std::cout << "\n";
    }
    db.commit_transaction();

    // SAFETY & DIRTY LISTS
    db.begin_transaction();
    db.add_node("Person", {{"name", std::string("Ghost")}, {"age", uint64_t(999)}}, 2030, MAX_TIME, 9999);
    db.abort_transaction();
    db.begin_transaction();
    std::cout << "\nNode Test (LID 9999):\n";
    for (uint64_t t : {2029, 2030, 2031, 999999}) std::cout << "  t=" << t << ": " << (db.try_get_node_at_vt(9999, t).has_value() ? "FAIL" : "OK") << "\n";
    db.commit_transaction();

    db.begin_transaction();
    db.add_relationship(a1, b1, "GHOST_REL", {{"tmp", uint64_t(1)}}, 2040, MAX_TIME, 99999);
    db.abort_transaction();
    db.begin_transaction();
    std::cout << "\nRelationship Test (LID 99999):\n";
    for (uint64_t t : {2039, 2040, 2041, 999999}) std::cout << "  t=" << t << ": " << (db.try_get_rship_at_vt(99999, t).has_value() ? "FAIL" : "OK") << "\n";
    db.commit_transaction();

    std::cout << "\n= COMMIT + ABORT UPDATE TEST (Alice LID 1) =\n";
    db.begin_transaction();
    db.update_node_create_version(stress_id, {{"age", uint64_t(999)}, {"debug_marker", uint64_t(999)}}, 2022);
    db.abort_transaction();
    db.begin_transaction();
    std::cout << "\nChecking visibility of Alice (LID 1) after aborted update:\n";
    for (uint64_t t : {2019, 2021, 2022, 2023}) {
        auto res = db.try_get_node_at_vt(1, t);
        if (!res.has_value()) continue;
        auto d = db.get_node_description(*res);
        bool leak = d.properties.count("debug_marker") || (d.properties.count("age") && std::any_cast<uint64_t>(d.properties.at("age")) == 999);
        if (leak) std::cout << "  At t=" << t << ": ERROR: Aborted update on Alice is VISIBLE!\n";
        else std::cout << "  At t=" << t << ": OK: Only committed Alice state visible.\n";
    }
    db.commit_transaction();

    return 0;
}