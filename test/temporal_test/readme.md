This directory contains the source code for the temporal extension of the Poseidon Graph Database, a memory-native engine designed for high-performance historical analysis. Unlike traditional databases that rely on destructive updates, this implementation enables fine-grained, object-level versioning and a native suite of time-travel query operators.

# Poseidon: Fine-Grained Temporal Versioning and Time-Travel

## Fine-Grained Object-Level Versioning
[cite_start]Poseidon moves away from coarse-grained snapshots and application-level version management to a native storage-level approach[cite: 51, 52].
* [cite_start]**Structural Versioning**: Every node and relationship struct is extended with explicit bi-temporal metadata, including valid-time intervals ($vt_{start}$, $vt_{end}$) and MVCC transaction timestamps ($bts$, $cts$)[cite: 478, 479, 480].
* [cite_start]**Unified Pointer Space**: Version history is maintained via a version-chain structure consisting of `prev_version` and `next_version` physical offsets embedded directly within object headers[cite: 481, 482, 483].
* [cite_start]**Tagged ID Resolution**: The engine utilizes a Tagged Pointer mechanism, reserving the most significant bit (MSB) of 64-bit physical entity IDs as a **HISTORY FLAG**[cite: 349, 350].
* [cite_start]**Zero-Overhead Routing**: During pointer-driven traversals, a bitwise AND mask evaluates this flag to distinguish between "hot" current storage and "cold" historical records with zero lookup overhead[cite: 351, 352, 353].
* [cite_start]**Adaptive Anchor + Delta Strategy**: To solve the storage-performance paradox, the system implements an Anchor and Delta strategy where full materialized states (**Anchors**) are periodically stored and linked to compact differential records (**Deltas**)[cite: 321, 322, 323, 324].
* [cite_start]**Mathematical Interval Optimization**: The frequency of anchor materialization is governed by a square-root model ($u(N) = \phi \cdot \sqrt{N}$), ensuring that storage overhead for highly active entities grows at a sub-linear rate relative to update volume[cite: 631, 632, 635, 649].

## Unified Temporal Indexing
[cite_start]Historical retrieval is accelerated by a Unified Temporal Index, a high-speed resolution layer that bypasses the need to traverse long, linear version chains[cite: 663, 664].
* [cite_start]**Composite Key Encoding**: The index uses a 64-bit unified key that packs a Partition Bit (Node vs Relationship), the stable Logical ID (LID), and the Valid Time Start timestamp into a single searchable integer[cite: 666, 672, 673, 676, 680].
* [cite_start]**Spatial Clustering**: By placing the LID in the higher-order bits, all temporal versions of a specific entity are physically clustered in the B+ tree leaf nodes, transforming random historical lookups into hardware-efficient sequential scans[cite: 677, 678, 679].
* [cite_start]**Direct Physical Resolution**: The index maps query parameters directly to 56-bit physical memory offsets and storage tier indicators, eliminating the heavy serialization and de-serialization cycles found in layered systems[cite: 684, 686, 700].

## Time-Travel Query Operator Suite
[cite_start]The engine provides a specialized suite of physical operators designed to reconstruct and traverse historical topologies at hardware-native speeds[cite: 720, 722].

### Temporal State Access
* [cite_start]**Temporal As-Of Scan ($\sigma_{T}$)**: Probes the Unified Temporal Index to return the precise physical memory offset for an entity valid at target time $t_{q}$[cite: 768, 772, 776].
* [cite_start]**Thread Pool Parallelization**: Historical snapshot materialization is distributed across multiple CPU cores, allowing the system to concurrently resolve version chains and apply incremental deltas for thousands of entities simultaneously[cite: 787, 788].
* **Timeline Scan**: Retrieves the complete evolutionary lineage of an entity over a specified time window ($W$), utilizing a "Topological Jump" and bounded backward walks to minimize processing overhead[cite: 821, 822, 826].

### Temporal Topological Expansion
* [cite_start]**Temporal Expand ($\infty_{T}$)**: Discovers adjacent nodes validly connected at $t_{q}$[cite: 863]. [cite_start]It utilizes **Temporal Pruning** to evaluate edge validity in-place via header-embedded timestamps before initiating target node resolution[cite: 869, 872, 873].
* **Variable-Length Expand ($\infty_{T}^{*}$)**: Generalizes expansion into a bounded breadth-first search (BFS) up to K hops, enforcing uniform temporal consistency across every level of the traversal[cite: 883, 890, 892].
* **Temporal Optional Expand ($\infty_{T}^{opt}$)**: Native implementation of outer-join (OPTIONAL MATCH) semantics for historical snapshots, ensuring structural continuity in query pipelines even when relationships are absent at $t_{q}$[cite: 945, 946, 947].

### Evolutionary and Relational Operators
* [cite_start]**Time-Slice Expand ($\mathbb{X}_{W}$)**: Streams all structural events and connectivity changes incident to a node that overlapped with a given time window[cite: 967, 968].
* [cite_start]**Relational Processing Layer**: Includes time-agnostic **Hash Joins**, **Left Joins**, **Top-K**, **Filter**, and **Project** operators that process materialized historical snapshots as standard relational tuples[cite: 1003, 1007, 1047, 1060, 1091, 1110].

## Append-Only Historical Storage
[cite_start]Historical data is migrated asynchronously to an archival tier designed to preserve physical connectivity without performance penalties[cite: 517, 518].
* **High-Density Chunks**: Data is stored as compact 48-byte packed structs appended directly into contiguous memory chunks or disk-backed pages, entirely avoiding serialization overhead[cite: 543, 544].
* [cite_start]**Pluggable Persistence**: The system supports a template-driven orchestration layer that allows users to toggle between **RAM Mode** (volatile chunked vectors) and **Disk Mode** (buffered, persistent pages)[cite: 594, 595, 596, 598].
