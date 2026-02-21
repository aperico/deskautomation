# Safety Goals

## Overview

This document captures the safety goals derived from the Hazard Analysis and Risk Assessment (HARA) and provides the authoritative safety goal list for downstream functional safety requirements and verification.

**Document Version:** 1.0  
**Date:** February 21, 2026  
**Status:** Draft

---

## Traceability

**Derived from:** [02_02_HARA-complete.md](02_02_HARA-complete.md)

---

## Safety Goals

| Safety Goal ID | Safety Goal Statement | Related Hazards | AgPLr Target |
|---------------|-----------------------|----------------|--------------|
| SG-001 | Prevent any unintended motion when no valid user command is present. | HZ-001, HZ-002, HZ-007 | b |
| SG-002 | Ensure motion stops within 500 ms of button release. | HZ-003 | b |
| SG-003 | Prevent motion beyond upper and lower travel limits. | HZ-004 | a |
| SG-004 | Prevent motion on conflicting UP/DOWN inputs. | HZ-005 | a |
| SG-005 | Prevent or safely stop motor runaway due to electrical faults or stuck-on driver outputs. | HZ-006, HZ-009 | b |
| SG-006 | Prevent unintended motion following brownout or reset events. | HZ-008 | a |

---

## Notes

- Safety goals are confirmed during the HARA workshop and maintained by the Functional Safety Engineer.
- Functional Safety Requirements derived from these goals are recorded in [03_00_SystemRequirements.md](03_00_SystemRequirements.md).
