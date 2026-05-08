# Project Management Documentation – LARC OPEN 2026

Hector Tovar served as the Project Manager for the LARC Open 2026 preparation cycle (January – April 2026, TMR).

The timeline Gantt chart for the TMR preparation cycle is available at [TImeline TMR 2026.png](../../../assets/LARC/TImeline%20TMR%202026.png).

---

## 1. Management Approach

The team ran **weekly group meetings** throughout the entire preparation cycle.

- **Meeting frequency**: Once per week.
- **Meeting duration**: 1 hour maximum per session. The **first session was 2 hours** and served as a full onboarding: introduction to LARC Open as a competition, explanation of each robot subsystem, roles, and how the team would operate for the cycle.
- **Meeting structure** (every session after the first):
  1. Review of tasks assigned the previous week — what was completed, what wasn't.
  2. New blockers surfaced by each sub-team.
  3. Definition of tasks for the coming week, assigned explicitly to each area: **Programming, Electronics, Mechanics, and PM**.
- **Tracking tool**: Miro board used for task tracking and sprint visualization.
- **Documentation**: No formal technical documentation was maintained during the project. This document is written in retrospect.

---

## 2. Project Timeline

The preparation window ran from **January 2026** through **late April 2026** (TMR).

### January – Base Setup

The focus of January was to establish the foundation that every other subsystem depended on: a physical chassis.

| Area | Work done |
|---|---|
| Mechanics | CAD design started; objective was to deliver a functional chassis to Programming as quickly as possible |
| Electronics | Reused the 2025 competition PCB board to have something to work with from day one, avoiding a blank-slate electronics start |
| Programming | Began work on low-level motor control for the existing base once the chassis was available (last week of January) |

### February – Demo Day & First Wiring Iteration

The **Demo Day was held on Saturday, February 7**. The requirement for that demo was simple: **the robot had to drive a square**. This served as a first integration checkpoint.

| Period | Activity |
|---|---|
| Pre-demo | Weekly check-ins focused on chassis progress; integration of electronics on the first mechanical iteration |
| **Feb 7** | **Demo Day 1** — robot drove a square autonomously |
| Post-demo | Started advancing the next chassis design iteration |
| Post-demo | Began cabling work for the robot (physical wiring and connectors) |
| Post-demo | Electronics continued developing on top of the 2025 PCB, keeping it as the main board through Regional |

### March – Regional & System Refactor

| Period | Activity |
|---|---|
| Week 1 | **Regional Competition** |
| Post-regional | Vision and communication subsystems were consistently failing; the team decided to refactor both for a more stable architecture |
| Post-regional | Started prototyping a new final PCB — this turned out to be a mistake (see retrospective) |
| Semana Santa (Holy Week) | New PCB arrived but did not work; the week was used instead to **solder and reorganize the 2025 PCB**, applying improvements to make it competition-ready |

### April – TMR

| Period | Activity |
|---|---|
| Pre-TMR | Integration of refactored vision and communication; continued improvements on the reorganized PCB |
| **Late April** | **TMR (Torneo Mexicano de Robótica)** — final competition |

---

## 3. Goals, Modifications & Results

### Goals set at project start

1. **Complete the full LARC Open track** autonomously.
2. **Win first place** at TMR.
3. Qualify and be ready for **LARC Internacional in Bogotá, Colombia**.

### How goals evolved

- Completing the full track remained the primary technical goal throughout. Electronics limitations in the final weeks meant the robot could not reliably run the complete course, though the overall system state was solid and the objective remained close.
- After Regional, a system stability goal was added: the vision and communication refactor became a priority after those subsystems were identified as the main failure point.
- The PCB replacement goal was added post-regional but effectively failed — the new board did not work, and the team pivoted back to improving the 2025 board.

### Achievement status

| Goal | Status |
|---|---|
| Complete the full track | Not fully achieved — electronics limitations prevented a full run, but the robot was in good shape |
| Win first place at TMR | **Achieved** |
| Ready for LARC Internacional (Bogotá, Colombia) | **Achieved** |

---

## 4. What Could Have Been Improved (Management Retrospective)

- **The new PCB was a late and risky bet.** Deciding to design and order a completely new PCB after Regional, with TMR weeks away, left no margin for iteration. When the board arrived non-functional during Semana Santa, the team had no time to debug it and had to fall back to the old board. The right call would have been to improve the 2025 PCB incrementally from the start rather than replacing it entirely under time pressure.

- **No documentation was maintained during the project.** Decisions, design rationale, and subsystem changes were never written down as they happened. This makes knowledge transfer to future teams harder and means institutional memory lives only in team members' heads.

- **Vision and communication failures at Regional were a foreseeable risk.** These subsystems showed instability before Regional but were not treated as a blocker. A more proactive risk policy would have flagged them earlier and allocated recovery time before the competition rather than after.

- **Blockers between sub-teams were surfaced reactively.** Dependencies (e.g., chassis dimensions affecting PCB mounting, cabling constraints tied to mechanical layout) were only discovered in weekly meetings rather than being pre-mapped at the start of each phase.

- **The weekly meeting format worked well overall.** The fixed structure — last week review, blockers, next tasks per area — kept everyone aligned with low overhead. One hour per week was sustainable and attendance was consistent.
