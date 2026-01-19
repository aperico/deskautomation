# System Objectives

**Document ID:** 02_SystemObjectives  
**Version:** 1.0  
**Date:** January 19, 2026  
**Status:** Draft  
**Author:** System Engineering Team  
**Compliance:** ISO 25119, ASPICE SYS.2, IEEE 29148-2018

---

## Document History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-01-19 | System Engineering Team | Initial objectives definition |

---

## Purpose

This document defines the high-level objectives for the Automated Mechanical Desk Lift System, establishing measurable goals that guide system design, implementation, and verification activities.

---

## Overview

System objectives translate the mission statement into specific, measurable, achievable, relevant, and time-bound (SMART) goals. Each objective is traceable to stakeholder needs and will be verified through defined metrics.

---

## System Objectives

### 1. Functional Objectives

#### OBJ-FUNC-01: Motorized Height Adjustment
**Statement:** Enable automated desk height adjustment through motorized control replacing manual cranking.

**Rationale:** Core functional capability - primary user need.

**Success Criteria:**
- Desk moves upward when rocker switch in UP position
- Desk moves downward when rocker switch in DOWN position
- Movement stops when switch in OFF position
- Movement range: full mechanical desk travel (≥30 cm vertical)
- Movement speed: 10 RPM motor → ~1.5 cm/sec vertical (TBD based on gear ratio)

**Verification:** System Integration Test

---

#### OBJ-FUNC-02: Simple User Interface
**Statement:** Provide intuitive, minimal-training operation through a single rocker switch.

**Rationale:** Usability - system must be easier than manual operation.

**Success Criteria:**
- Single rocker switch with 3 states (UP, OFF, DOWN)
- Switch position UP initiates upward movement
- Switch position DOWN initiates downward movement
- Switch position OFF stops all movement
- No configuration required for basic operation

**Verification:** Usability Test, System Test

---

#### OBJ-FUNC-03: Reliable State Management
**Statement:** Maintain accurate system state across all operational modes and transitions.

**Rationale:** Predictable behavior essential for user trust and safety.

**Success Criteria:**
- System boots to known safe IDLE state
- State transitions follow defined state machine (see SoftwareArchitecture.md)
- No undefined states or race conditions
- State persistence during power loss (safe recovery)

**Verification:** Unit Test, Integration Test

---

### 2. Safety Objectives

#### OBJ-SAFE-01: Emergency Stop Capability
**Statement:** Provide immediate motor shutdown through dedicated emergency stop mechanism.

**Rationale:** Critical safety feature - ASIL-B level (ISO 25119).

**Success Criteria:**
- Emergency stop detection within 10 ms
- Motor shutdown within 50 ms of emergency stop activation
- System enters FAULT state requiring power cycle
- Emergency stop overrides all other commands

**Verification:** Safety Test, Response Time Test

---

#### OBJ-SAFE-02: Overcurrent Protection
**Statement:** Detect and respond to motor overcurrent conditions indicating mechanical binding or stall.

**Rationale:** Prevent motor damage, detect mechanical faults, protect user from pinch hazards.

**Success Criteria:**
- Current monitoring via BTS7960 R_IS/L_IS sense pins
- Detection threshold: >5A (TBD based on motor nominal)
- Detection latency: <100 ms
- Response: immediate motor shutdown, enter FAULT state

**Verification:** Hardware-in-Loop Test, Fault Injection Test

---

#### OBJ-SAFE-03: Soft-Start Operation
**Statement:** Gradually ramp motor PWM to reduce mechanical shock and current spike.

**Rationale:** Extend component life, reduce wear, smoother operation.

**Success Criteria:**
- PWM ramp from 0 to target over 500-1000 ms
- Reduced inrush current (<3A peak)
- Smooth movement initiation (no jerking)

**Verification:** Integration Test, Current Measurement

---

#### OBJ-SAFE-04: Fail-Safe Design
**Statement:** System shall fail to safe state (motor stopped) in all fault scenarios.

**Rationale:** Safety-critical principle - prevent unintended movement.

**Success Criteria:**
- Watchdog timer protection
- Safe state on power loss
- Safe state on software fault
- Safe state on hardware fault (motor driver, sensor)

**Verification:** Fault Tree Analysis, FMEA, Fault Injection Tests

---

### 3. Quality Objectives

#### OBJ-QUAL-01: Modularity and Testability
**Statement:** Implement layered architecture with hardware abstraction enabling host-based testing.

**Rationale:** Support unit testing without hardware, enable CI/CD, reduce development cost.

**Success Criteria:**
- Hardware Abstraction Layer (HAL) isolates hardware dependencies
- Unit test coverage ≥80% for business logic
- GoogleTest framework integration
- CMake build system for host and target builds

**Verification:** Code Review, Test Coverage Report

---

#### OBJ-QUAL-02: ASPICE Compliance
**Statement:** Follow Automotive SPICE (ASPICE) development processes for requirements management, design, and verification.

**Rationale:** Industry best practice for safety-critical systems, ensure traceability and quality.

**Success Criteria:**
- ASPICE Level 2+ capability for relevant process areas
- Complete traceability matrix (requirements → design → code → test)
- Document templates compliant with ISO 25119
- Regular process assessments

**Verification:** Process Assessment, Document Audit

---

#### OBJ-QUAL-03: Standards Compliance
**Statement:** Comply with relevant safety and development standards.

**Rationale:** Legal/regulatory requirements, safety assurance, industry best practices.

**Success Criteria:**
- ISO 25119 (Agricultural machinery control systems)
- IEEE 29148-2018 (Requirements Engineering)
- ISO/IEC 12207 (Software Life Cycle Processes)
- INCOSE requirements principles

**Verification:** Standards Gap Analysis, Compliance Audit

---

### 4. Performance Objectives

#### OBJ-PERF-01: Response Time
**Statement:** System shall respond to user inputs within acceptable latency limits.

**Rationale:** User experience, safety (emergency stop).

**Success Criteria:**
- Switch state detection: <50 ms
- Motor start: <100 ms from switch actuation
- Emergency stop: <50 ms detection + motor shutdown

**Verification:** Performance Test, Timing Analysis

---

#### OBJ-PERF-02: Reliability
**Statement:** System shall operate reliably over extended lifetime.

**Rationale:** User satisfaction, maintenance cost reduction.

**Success Criteria:**
- MTBF (Mean Time Between Failures): >10,000 movement cycles
- No software crashes or undefined behavior
- Graceful degradation on component failure
- Lifetime: 5+ years normal use

**Verification:** Endurance Test, FMEA, Reliability Analysis

---

#### OBJ-PERF-03: Resource Efficiency
**Statement:** Implement system within Arduino UNO resource constraints.

**Rationale:** Cost, hardware availability, project feasibility.

**Success Criteria:**
- Flash memory: <75% of 32 KB
- SRAM: <75% of 2 KB
- CPU utilization: <50% average
- No dynamic memory allocation (stack-only)

**Verification:** Resource Profiling, Static Analysis

---

### 5. Extensibility Objectives

#### OBJ-EXT-01: Future Feature Readiness
**Statement:** Design architecture to accommodate anticipated future enhancements.

**Rationale:** Product evolution, user feedback incorporation, competitive features.

**Planned Extensions:**
- Height position sensing (potentiometer/encoder)
- Height memory presets (3-4 favorite positions)
- Mobile app control (Bluetooth/WiFi)
- Multi-user profiles
- Sitting/standing timer and reminders
- Obstacle detection (ultrasonic/pressure sensor)

**Success Criteria:**
- Modular architecture supports feature addition without major refactoring
- Sufficient reserved I/O pins
- Software architecture supports extension points

**Verification:** Architecture Review, Extensibility Analysis

---

## Objectives Traceability

| Objective | Mission Element | Stakeholder Need | Verification Method |
|-----------|-----------------|------------------|---------------------|
| OBJ-FUNC-01 | TO | End User | System Test |
| OBJ-FUNC-02 | TO, WHILE | End User | Usability Test |
| OBJ-FUNC-03 | BY | Developer, End User | Unit/Integration Test |
| OBJ-SAFE-01 | WHILE | End User, Safety Team | Safety Test |
| OBJ-SAFE-02 | WHILE | End User, Safety Team | HiL Test |
| OBJ-SAFE-03 | BY | End User, Hardware | Integration Test |
| OBJ-SAFE-04 | WHILE | Safety Team | FMEA, FTA |
| OBJ-QUAL-01 | BY, WHILE | Developer | Code Review |
| OBJ-QUAL-02 | WHILE | System Engineer | Process Assessment |
| OBJ-QUAL-03 | WHILE | System Engineer, Safety Team | Compliance Audit |
| OBJ-PERF-01 | TO | End User | Performance Test |
| OBJ-PERF-02 | WHILE | End User | Reliability Analysis |
| OBJ-PERF-03 | WHILE | Developer | Resource Profiling |
| OBJ-EXT-01 | WHILE | End User, Developer | Architecture Review |

---

## Objectives Summary Dashboard

| Category | Total | Critical | High | Medium |
|----------|-------|----------|------|--------|
| Functional | 3 | 2 | 1 | 0 |
| Safety | 4 | 4 | 0 | 0 |
| Quality | 3 | 0 | 3 | 0 |
| Performance | 3 | 1 | 2 | 0 |
| Extensibility | 1 | 0 | 0 | 1 |
| **Total** | **14** | **7** | **6** | **1** |

---



## References

- [Mission Statement](01_MissionStatement.md)
- [Concept of Operations](03_ConOps.md)
- [System Context Diagram](04_SystemContextDiagram.md)
- [System Use Cases](05_SystemUseCases.md)
- [Software Requirements](07_SoftwareRequirements.md)
- [Traceability Matrix](12_TraceabilityMatrix.md)

---

**Approval Status:** Pending Review

| Role | Name | Signature | Date |
|------|------|-----------|------|
| System Engineer | TBD | | |
| Safety Representative | TBD | | |
| Requirements Manager | TBD | | |
| Project Lead | TBD | | |
