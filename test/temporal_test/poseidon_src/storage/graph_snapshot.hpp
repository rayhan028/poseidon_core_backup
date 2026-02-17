#ifndef GRAPH_SNAPSHOT_HPP
#define GRAPH_SNAPSHOT_HPP
#include "nodes.hpp"
#include "relationships.hpp"
#include "properties.hpp"
#include <cstdint>
#include <vector>
#include <mutex>

// Metadata for each node in the snapshot
struct NodeSnapshotMeta {
    bool     from_checkpoint{false}; // Indicates if the node was sourced from a checkpoint
    uint64_t materialized_at_vt{0}; // The valid time at which the node was materialized
};

struct RshipSnapshotMeta {
    bool     from_checkpoint{false}; // Indicates if the relationship was sourced from a checkpoint
    uint64_t materialized_at_vt{0}; // The valid time at which the relationship was materialized
};

// A complete snapshot of the graph at a specific time
struct GraphSnapshot {
    std::mutex snap_mtx; // Mutex for thread-safe vector access

    std::vector<node_description>     nodes; // All nodes in the snapshot
    std::vector<rship_description>    relationships; // All relationships in the snapshot

    std::vector<NodeSnapshotMeta>     node_meta; // Metadata for each node
    std::vector<RshipSnapshotMeta>    rship_meta; // Metadata for each relationship

    GraphSnapshot() = default; // Default constructor

    // Move Constructor: Moves the data, ignores the old mutex
    GraphSnapshot(GraphSnapshot&& other) noexcept
        : nodes(std::move(other.nodes)), // Move nodes
          relationships(std::move(other.relationships)), // Move relationships
          node_meta(std::move(other.node_meta)), // Move node metadata
          rship_meta(std::move(other.rship_meta)) // Move relationship metadata
    {
        // The new snap_mtx is default-constructed automatically
    }

    // disable copy operations to avoid accidental copies
    GraphSnapshot(const GraphSnapshot&) = delete; // Copy constructor
    GraphSnapshot& operator=(const GraphSnapshot&) = delete; // Copy assignment operator
};

#endif // GRAPH_SNAPSHOT_HPP