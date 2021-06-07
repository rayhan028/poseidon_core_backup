#ifndef POSEIDON_CORE_JOINER_HPP
#define POSEIDON_CORE_JOINER_HPP

#include <vector>
#include "global_definitions.hpp"

using input = std::vector<qr_tuple>;
using id_input = std::vector<offset_t>;

class joiner {
public:
    joiner() = default;
    
    std::map<int, qr_tuple> mat_tuple_;
    std::map<int, input> rhs_input_;
    std::map<int, id_input> id_input_;

    std::map<int, input> rhs_hash_input_[10];
    std::map<int, id_input> id_hash_input_[10];

    std::mutex materialize_mutex;
    void materialize_rhs(int jid, qr_tuple *qr);

    void materialize_rhs_id(int jid, offset_t id);

    void materialize_rhs_hash_join(int jid, int bucket, qr_tuple *qr);
    void materialize_rhs_id_hash_join(int jid, int bucket, int id);

    int get_input_size(int jid, int bucket);
    int get_input_id(int jid, int bucket, int idx);
    qr_tuple * get_query_result(int jid, int bucket, int idx);
};


#endif //POSEIDON_CORE_JOINER_HPP
