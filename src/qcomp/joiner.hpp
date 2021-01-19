#ifndef POSEIDON_CORE_JOINER_HPP
#define POSEIDON_CORE_JOINER_HPP

#include <vector>
#include "global_definitions.hpp"
using input = std::vector<qr_tuple>;
using id_input = std::vector<offset_t>;

class joiner {
public:
    joiner();
    
    static std::map<int, qr_tuple> mat_tuple_;
    static std::map<int, input> rhs_input_;
    static std::map<int, id_input> id_input_;

    static std::map<int, input> rhs_hash_input_[10];
    static std::map<int, id_input> id_hash_input_[10];

    static std::mutex materialize_mutex;
    static void materialize_rhs(int jid, qr_tuple *qr);

    static void materialize_rhs_id(int jid, offset_t id);

    static void materialize_rhs_hash_join(int jid, int bucket, qr_tuple *qr);
    static void materialize_rhs_id_hash_join(int jid, int bucket, int id);

    static int get_input_size(int jid, int bucket);
    static int get_input_id(int jid, int bucket, int idx);
    static qr_tuple * get_query_result(int jid, int bucket, int idx);
};


#endif //POSEIDON_CORE_JOINER_HPP
