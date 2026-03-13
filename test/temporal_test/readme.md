This directory contains the source code for the temporal extension of the Poseidon Graph Database, a memory-native engine designed for high-performance historical analysis. Unlike traditional databases that rely on destructive updates, this implementation enables fine-grained, object-level versioning and a native suite of time-travel query operators.

# Poseidon: Fine-Grained Temporal Versioning and Time-Travel

## Fine-Grained Object-Level Versioning
Poseidon moves away from coarse-grained snapshots and application-level version management to a native storage-level approach.

* **Structural Versioning**: Every node and relationship struct is extended with explicit bi-temporal metadata, including valid-time intervals ($vt_{start}$, $vt_{end}$) and MVCC transaction timestamps ($bts$, $cts$).
* **Unified Pointer Space**: Version history is maintained via a version-chain structure consisting of `prev_version` and `next_version` physical offsets embedded directly within object headers.
* **Tagged ID Resolution**: The engine utilizes a Tagged Pointer mechanism, reserving the most significant bit (MSB) of 64-bit physical entity IDs as a **HISTORY FLAG** ($1 \ll 63$).
* **Zero-Overhead Routing**: During pointer-driven traversals, a bitwise AND mask evaluates this flag to distinguish between "hot" current storage and "cold" historical records with zero lookup overhead.
* **Adaptive Anchor + Delta Strategy**: To solve the storage-performance paradox, the system implements an Anchor and Delta strategy where full materialized states (**Anchors**) are periodically stored and linked to compact differential records (**Deltas**).
* **Mathematical Interval Optimization**: The frequency of anchor materialization is governed by a square-root model ($u(N) = \phi \cdot \sqrt{N}$), ensuring that storage overhead for highly active entities grows at a sub-linear rate relative to update volume.

## Unified Temporal Indexing
Historical retrieval is accelerated by a Unified Temporal Index, a high-speed resolution layer that bypasses the need to traverse long, linear version chains.

* **Composite Key Encoding**: The index uses a 64-bit unified key that packs a Partition Bit (Node vs Relationship), the stable Logical ID (LID), and the Valid Time Start timestamp into a single searchable integer.
* **Spatial Clustering**: By placing the LID in the higher-order bits, all temporal versions of a specific entity are physically clustered in the B+ tree leaf nodes, transforming random historical lookups into hardware-efficient sequential scans.
* **Direct Physical Resolution**: The index maps query parameters directly to 56-bit physical memory offsets and storage tier indicators, eliminating the heavy serialization and de-serialization cycles found in layered systems.

## Time-Travel Query Operator Suite
The engine provides a specialized suite of physical operators designed to reconstruct and traverse historical topologies at hardware-native speeds.

### Temporal State Access
* **Temporal As-Of Scan ($\sigma_{T}$)**: Probes the Unified Temporal Index to return the precise physical memory offset for an entity valid at target time $t_{q}$.
* **Thread Pool Parallelization**: Historical snapshot materialization is distributed across multiple CPU cores, allowing the system to concurrently resolve version chains and apply incremental deltas for thousands of entities simultaneously.
* **Timeline Scan**: Retrieves the complete evolutionary lineage of an entity over a specified time window ($W$), utilizing a "Topological Jump" and bounded backward walks to minimize processing overhead.

### Temporal Topological Expansion
* **Temporal Expand ($\infty_{T}$)**: Discovers adjacent nodes validly connected at $t_{q}$. It utilizes **Temporal Pruning** to evaluate edge validity in-place via header-embedded timestamps before initiating target node resolution.
* **Variable-Length Expand ($\infty_{T}^{*}$)**: Generalizes expansion into a bounded breadth-first search (BFS) up to K hops, enforcing uniform temporal consistency across every level of the traversal.
* **Temporal Optional Expand ($\infty_{T}^{opt}$)**: Native implementation of outer-join (OPTIONAL MATCH) semantics for historical snapshots, ensuring structural continuity in query pipelines even when relationships are absent at $t_{q}$.

### Evolutionary and Relational Operators
* **Time-Slice Expand ($\mathbb{X}_{W}$)**: Streams all structural events and connectivity changes incident to a node that overlapped with a given time window.
* **Relational Processing Layer**: Includes time-agnostic **Hash Joins**, **Left Joins**, **Top-K**, **Filter**, and **Project** operators that process materialized historical snapshots as standard relational tuples.

## Append-Only Historical Storage
Historical data is migrated asynchronously to an archival tier designed to preserve physical connectivity without performance penalties.

* **High-Density Chunks**: Data is stored as compact 48-byte packed structs appended directly into contiguous memory chunks or disk-backed pages, entirely avoiding serialization overhead.
* **Pluggable Persistence**: The system supports a template-driven orchestration layer that allows users to toggle between **RAM Mode** (volatile chunked vectors) and **Disk Mode** (buffered, persistent pages).
