# Interplanetary File Sharing System with SHA1

This project was created as part of the Final Project for Data Structures (Fall 2023).

## Contributors:
Ayaan Khan
Minahil Ali
Mustafa Kamran

## Overview

This project is a decentralized file sharing system implemented in C++. Inspired by the Interplanetary File System (IPFS), it enables efficient, distributed file transfer and retrieval. The system ensures data integrity using SHA1 hashing and utilizes advanced data structures like B-Trees and Circular Distributed Hash Tables (DHTs) for scalable, fast, and reliable peer-to-peer networking.

---

## Features

- **Decentralized Peer-to-Peer Architecture:** No central server is required. Nodes connect and share files directly.
- **SHA1 Hashing:** Files and file chunks are hashed with SHA1 for integrity verification and unique identification.
- **B-Tree Indexing:** B-Trees are used to index and efficiently manage file metadata and chunk locations.
- **Circular DHT (Distributed Hash Table):** Each peer participates in a circular DHT for scalable and fault-tolerant storage and lookup, enabling efficient resource discovery and routing.
- **Chunked File Transfer:** Files are split into chunks for transfer and deduplication.
- **Peer Discovery & Bootstrap:** Nodes can join or leave the network dynamically, discovering other peers through the DHT.
- **Data Integrity:** Every file chunk is verified using its SHA1 hash after transfer.

---

## Data Structures

### B-Tree

- **Usage:** B-Trees are implemented to store and index file metadata, such as mappings from file names to their corresponding SHA1 hashes and chunk locations.
- **Benefits:** Quick search, insert, and delete operations, allowing the system to handle large numbers of files and chunks efficiently.

### Circular DHT (Distributed Hash Table)

- **Usage:** The system uses a Chord-like circular DHT for peer management and file chunk distribution.
- **Benefits:** Scalable, fault-tolerant, and supports efficient O(log N) lookups for locating the peer responsible for any given file/chunk hash.

---

## How It Works

1. **File Addition:**  
   - The file is split into chunks.
   - Each chunk is hashed (SHA1).
   - The B-Tree indexes the mapping between file names, chunk hashes, and their locations.
   - Chunks and their hashes are advertised to the DHT.

2. **File Discovery & Download:**  
   - A peer queries the DHT with the SHA1 hash to find which node(s) hold the chunk.
   - Upon retrieval, each chunk's integrity is verified via SHA1.
   - Chunks are reassembled into the original file.

3. **Peer Management:**  
   - The DHT ensures that any peer can efficiently discover and connect to nodes responsible for specific hash ranges.
   - When a peer joins or leaves, the DHT and B-Tree structures rebalance as needed.


