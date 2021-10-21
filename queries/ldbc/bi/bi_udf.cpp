// bi1
auto isComment = [&](auto res) {
    return pj::has_label(res, "Comment") ?
    query_result(std::string("True")) : query_result(std::string("False"));
};

auto lengthCategory = [&](auto res) {
    auto len = boost::get<int>(res);
    return (len >= 0 && len < 40) ? query_result(std::string("0")) :
            (len >= 40 && len < 80) ? query_result(std::string("1")) :
            (len >= 80 && len < 160) ? query_result(std::string("2")) :
            query_result(std::string("3"));
};

// bi2
auto timePeriod1 = [&](auto res) {
    auto d = boost::get<ptime>(res);
    time_period duration(%date, hours(24*100));
    return duration.contains(d) ? true : false;
};

auto timePeriod2 = [&](auto res) {
    auto d = boost::get<ptime>(res);
    time_period duration1(%date, hours(24*100));
    auto dt2 = duration1.last();
    time_period duration2(dt2, hours(24*100));
    return duration2.contains(d) ? true : false;
};

auto countDiff = [&](auto &v) {
    auto cnt = boost::get<uint64_t>(v[1]);
    auto nxt_cnt = boost::get<uint64_t>(v[2]);
    uint64_t diff = cnt > nxt_cnt ? cnt - nxt_cnt : nxt_cnt - cnt;
    return query_result(diff);
});

// bi5
auto score = [&](auto &v) {
    auto reply_cnt = boost::get<uint64_t>(v[1]);
    auto like_cnt = boost::get<uint64_t>(v[3]);
    auto msg_cnt = boost::get<uint64_t>(v[5 ]);
    auto score = msg_cnt + 2 * reply_cnt + 10 * like_cnt;
    return query_result(score);
};

// bi8
auto append100 = [&](auto &v) {
    uint64_t intrst = 100;
    return query_result(intrst);
};

// bi9
auto incrMsgCnt = [&](auto &v) {
    uint64_t intrst = 100;
    return query_result(intrst);
};

// bi11
auto samePerson = [&](auto res1, auto res2) {
    return boost::get<uint64_t>(res1) == boost::get<uint64_t>(res2);
};

auto distinctBag = [&](auto &v) {
    auto a = boost::get<node *>(v[0])->id();
    auto b = boost::get<node *>(v[1])->id();
    auto c = boost::get<node *>(v[2])->id();
    if (a > c)
        std::swap(a, c);
    if (a > b)
        std::swap(a, b);
    if (b > c)
        std::swap(b, c);
    std::string key = std::to_string(a) + "_" + std::to_string(b) + "_" + std::to_string(c);
    return query_result(key);
});

// bi12
auto hasContent1 = [&](auto res) {
    return pj::has_property(res, "content") ?
    query_result(std::string("1")) : query_result(std::string("0"));
};

auto hasContent2 = [&](auto res) {
    return pj::has_property(res, "content") ?
    pj::string_property(res, "language") : query_result("n/a");
};

auto postFeatures = [&](auto &v) {
    if (boost::get<int>(v[1]) == 0 ||
        boost::get<int>(v[2]) >= boost::get<int>(params[1]) ||
        boost::get<ptime>(v[3]) <= boost::get<ptime>(params[0]))
            return false;
    for (std::size_t i = 2; i < params.size(); i++)
        if (boost::get<std::string>(v[4]) == boost::get<std::string>(params[i]))
            return true;
    return false;
});

auto cmtFeatures = [&](auto &v) {
    if (boost::get<int>(v[2]) == 0 ||
        boost::get<int>(v[3]) >= boost::get<int>(params[1]) ||
        boost::get<ptime>(v[4]) <= boost::get<ptime>(params[0]))
            return false;
    else
        return true;
});

auto joinPred = [&](auto &lv, auto &rv) {
    return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id();
});

auto appendSum = [&](auto &v) {
    return v[1].type() == typeid(null_val) ?
        query_result((uint64_t)0) : query_result(boost::get<uint64_t>(v[2]));
};

// bi13
auto avgMsgs = [&](auto &v) {
    auto sdt = to_iso_extended_string(boost::get<ptime>(v[2]));
    auto ssyr = sdt.substr(0, sdt.find("-"));
    auto ssmo = sdt.substr(5, 2);
    auto syr = std::stoi(ssyr);
    auto smo = std::stoi(ssmo);
    auto edt = to_iso_extended_string(boost::get<ptime>(params[1]));
    auto esyr = edt.substr(0, edt.find("-"));
    auto esmo = edt.substr(5, 2);
    auto eyr = std::stoi(esyr);
    auto emo = std::stoi(esmo);
    auto msgs = boost::get<uint64_t>(v[1]);
    uint64_t months =
        (syr == eyr) ? (emo - smo + 1) : (12 - smo + 1) + ((eyr - syr) * 12) + emo;
    uint64_t avg_msg = msgs / months;
    return query_result(avg_msg);
};

auto joinPred = [&](auto &lv, auto &rv) {
    return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id();
});

auto zmbMsgCnt = [&](auto &v) {
    return v[4].type() == typeid(null_val) ? true :
            boost::get<uint64_t>(v[4]) < 1 ? true : false;
});

auto appendScore1 = [&](auto &v, int idx) {
    return v[idx].type() == typeid(null_val) ?
        query_result((uint64_t)0) : query_result(boost::get<uint64_t>(v[idx]));
});

auto appendScore2 = [&](auto &v) {
    double score = boost::get<uint64_t>(v[6]) == 0 ? 0.0 :
                    boost::get<uint64_t>(v[5]) / (double)boost::get<uint64_t>(v[6]); 
    return query_result(score);
});

// bi14
auto fltrRship = [&](auto &v, int idx, string label) {
    if (v[idx].type() == typeid(null_val)) 
        return false;
    auto r = boost::get<relationship *>(v[idx]);
    return r->rship_label == gdb->get_code(label) ? true : false;
});

auto knowsRship = [&](auto &v) {
    auto r = v[4].type() == typeid(relationship *) ? boost::get<relationship *>(v[4]) :
                v[5].type() == typeid(relationship *) ? boost::get<relationship *>(v[5]) : nullptr;
    return !r ? false : r->rship_label == gdb->get_code(":knows") ? true : false;
});

auto appendScore = [&](auto &v, int score) {
    return query_result((uint64_t)score);
});

// bi15
auto edge_weight = [&](const node &n1, const node &n2) {
    double w = 0.0;
    gdb->foreach_to_relationship_of_node(n1, ":hasCreator", [&](relationship &r1) {
        auto &msg1 = gdb->node_by_id(r1.from_node_id());
        if (msg1.node_label == gdb->get_code("Post")) {
        gdb->foreach_to_relationship_of_node(msg1, ":containerOf", [&](relationship &r2) {
            auto &forum = gdb->node_by_id(r2.from_node_id());
            if (forum.node_label == gdb->get_code("Forum")) {
            auto forum_descr = gdb->get_node_description(forum.id());
            ptime dt = get_property<ptime>(forum_descr.properties, 
                                                std::string("creationDate")).value();
            ptime gdt1 = boost::get<ptime>(params[2]);
            ptime gdt2 = boost::get<ptime>(params[3]);
            if (gdt1 <= dt && dt <= gdt2) {
                gdb->foreach_to_relationship_of_node(msg1, ":replyOf", [&](relationship &r3) {
                auto &msg2 = gdb->node_by_id(r3.from_node_id());
                if (msg2.node_label == gdb->get_code("Comment")) {
                    gdb->foreach_from_relationship_of_node(msg2, ":hasCreator", [&](relationship &r4) {
                    if (r4.to_node_id() == n2.id())
                        w += 1.0;
                    });
                }
                });
            }
            }
        });
        }
        else if (msg1.node_label == gdb->get_code("Comment")) {
        auto reply_code = gdb->get_code(":replyOf");
        gdb->foreach_variable_from_relationship_of_node(msg1, reply_code, 1, 100, [&](relationship &r2) {
            auto &post = gdb->node_by_id(r2.to_node_id());
            if (post.node_label == gdb->get_code("Post")) {
            gdb->foreach_to_relationship_of_node(post, ":containerOf", [&](relationship &r3) {
                auto &forum = gdb->node_by_id(r3.from_node_id());
                if (forum.node_label == gdb->get_code("Forum")) {
                auto forum_descr = gdb->get_node_description(forum.id());
                ptime dt = get_property<ptime>(forum_descr.properties, 
                                                    std::string("creationDate")).value();
                ptime gdt1 = boost::get<ptime>(params[2]);
                ptime gdt2 = boost::get<ptime>(params[3]);
                if (gdt1 <= dt && dt <= gdt2) {
                    gdb->foreach_to_relationship_of_node(msg1, ":replyOf", [&](relationship &r4) {
                    auto &msg2 = gdb->node_by_id(r4.from_node_id());
                    if (msg1.node_label == gdb->get_code("Comment")) {
                        gdb->foreach_from_relationship_of_node(msg2, ":hasCreator", [&](relationship &r5) {
                        if (r5.to_node_id() == n2.id())
                            w += 0.5;
                        });
                    }
                    });
                }
                }
            });
            }
        });
        }
    });
    return w;
};

auto rshipPred [&](relationship &r) {
    return std::string(gdb->get_string(r.rship_label)) == ":knows";
});

auto appendIds [&](auto &v) {
    auto arr = boost::get<array_t>(v[3]).elems;
    std::vector<offset_t> ids;
    for (auto id : arr) {
        auto &n = gdb->node_by_id(id);
        auto nd = gdb->get_node_description(n.id());
        auto p = get_property<uint64_t>(nd.properties, "id");
        ids.push_back(p.value());
    }
    array_t nids(ids);
    return query_result(nids);
});

auto appendWeight [&](auto &v) {
    double weight = 0.0;
    auto nids = boost::get<array_t>(v[3]).elems;
    for (std::size_t i = 0; i < (nids.size() - 1); i++) {
        auto &n1 = gdb->node_by_id(nids[i]);
        auto &n2 = gdb->node_by_id(nids[i + 1]);
        weight += edge_weight(n1, n2);
        weight += edge_weight(n2, n1);
    }
    return query_result(weight);
});

auto sortedId [&](auto &v) {
    auto a = boost::get<array_t>(q1[4]).elems;
    auto b = boost::get<array_t>(q2[4]).elems;
    auto m = std::mismatch(a.begin(), a.end(), b.begin(), b.end());
    return *(m.first) < *(m.second);
});

// bi16
auto joinPred = [&](auto &lv, auto &rv) {
    return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id();
});

auto fltrCnt = [&](auto &v) {
    if (v[3].type() == typeid(null_val)) 
        return true;
    return boost::get<uint64_t>(v[3]) <= (uint64_t)boost::get<int>(params[4]);
});

// bi17
auto samePerson = [&](auto &v, int idx1, int idx2) {
    return boost::get<node *>(v[idx1])->id() != boost::get<node *>(v[idx2])->id();
});

auto msgInterval = [&](auto &lv, auto &rv) {
    auto msg1_dt = boost::get<ptime>(v[0]);
    auto msg2_dt = boost::get<ptime>(v[5]);
    auto hrs = boost::get<int>(params[1]);
    return (msg1_dt + hours(hrs)) < msg2_dt
});

auto fltrRship1 = [&](auto &v, int idx, string label) {
    if (v[idx].type() == typeid(null_val)) 
        return false;
    auto r = boost::get<relationship *>(v[idx]);
    return r->rship_label == gdb->get_code(label) ? true : true;
});


auto fltrRship2 = [&](auto &v, int idx, string label) {
    if (v[idx].type() == typeid(null_val)) 
        return false;
    auto r = boost::get<relationship *>(v[idx]);
    return r->rship_label == gdb->get_code(label) ? false : true;
});

// bi18
auto knowsRship = [&](auto &v, int idx, string label) {
    if (boost::get<node *>(v[0])->id() == boost::get<node *>(v[4])->id())
    return false;
    relationship *r = v[5].type() == typeid(relationship *) ?
    boost::get<relationship *>(v[5]) : v[6].type() == typeid(relationship *) ?
    boost::get<relationship *>(v[6]) : nullptr;
    return !r ? true : r->rship_label == gdb->get_code(":knows") ?
    false : true;
});

// bi19
auto interaction = [&](const node &n1, const node &n2) {
    auto count = 0;
    gdb->foreach_to_relationship_of_node(n1, ":hasCreator", [&](relationship &r1) {
    auto &comment = gdb->node_by_id(r1.from_node_id());
    if (comment.node_label == gdb->get_code("Comment")) {
        gdb->foreach_from_relationship_of_node(comment, ":replyOf", [&](relationship &r2) {
        auto &msg = gdb->node_by_id(r2.to_node_id());
        if (msg.node_label == gdb->get_code("Post") || msg.node_label == gdb->get_code("Comment")) {
            gdb->foreach_from_relationship_of_node(msg, ":hasCreator", [&](relationship &r3) {
            if (r3.to_node_id() == n2.id())
                count++;
            });
        }
        });
    }
    });
    return count;
};

auto rweight = [&](relationship &r) {
    double w = 0.0;
    auto &src = gdb->node_by_id(r.from_node_id());
    auto &des = gdb->node_by_id(r.to_node_id());
    auto count = interaction(src, des) + interaction(des, src);
    return count == 0 ? 0 : 1 / (double)count;
};

auto rPred = [&](relationship &r) {
    return std::string(gdb->get_string(r.rship_label)) == ":knows";
};

// bi20
rship_predicate rpred = [&](relationship &r) {
    auto &src = gdb->node_by_id(r.from_node_id());
    auto &des = gdb->node_by_id(r.to_node_id());
    node::id_t src_uni, des_uni;
    gdb->foreach_from_relationship_of_node(src, ":studyAt", [&](relationship &r) {
    src_uni = r.to_node_id();
    });
    gdb->foreach_from_relationship_of_node(des, ":studyAt", [&](relationship &r) {
    des_uni = r.to_node_id();
    });
    bool a = std::string(gdb->get_string(r.rship_label)) == ":knows";
    bool b = src_uni == des_uni;
    return a && b;
};

rship_weight rweight = [&](relationship &r) {
    auto &src = gdb->node_by_id(r.from_node_id());
    auto &des = gdb->node_by_id(r.to_node_id());
    int src_yr, des_yr;
    gdb->foreach_from_relationship_of_node(src, ":studyAt", [&](relationship &r1) {
    auto descr = gdb->get_rship_description(r1.id());
    src_yr = get_property<int>(descr.properties, std::string("classYear")).value();
    });
    gdb->foreach_from_relationship_of_node(des, ":studyAt", [&](relationship &r2) {
    auto descr = gdb->get_rship_description(r2.id());
    des_yr = get_property<int>(descr.properties, std::string("classYear")).value();
    });
    double w = std::abs(src_yr - des_yr) + 1;
    return w;
};



