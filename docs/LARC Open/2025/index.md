# LARC Open 2025 — Coffee Harvester Robot (Robot Recolector de Café)

This page summarizes the LARC Open 2025 proposal "Robot Recolector de Café". The project
focuses on an autonomous ground robot that locates, picks and delivers coffee fruits/items in a
competition arena. The goal is a reliable, fast, and robust system that performs repeated
collection tasks with minimal human intervention.

**Objectives**
- Design an autonomous robot to detect and pick coffee items from the field and place them in
  a designated deposit zone.
- Operate without manual control during a timed run; the robot may start with an unknown
  initial pose and must handle variable item placements.
- Prioritize reliability: avoid damaging the environment, handle missed picks gracefully,
  and maximize the number of successfully delivered items.
This page contains a short summary and links to the official rules and technical details.

**More about this challenge**
- Context: LARC Open tasks require teams to design robots that work reliably in semi-structured
  environments. This specific challenge simulates the task of harvesting/collecting coffee
  items and transporting them to a deposit area within a limited time window.
- Focus areas: perception (detecting ripe items), navigation (safe traversal and obstacle
  avoidance), manipulation (reliable pick-and-place), and system integration (real-time control
  and recovery strategies).

**Rules & Scoring (summary)**
- Official rules and full scoring details are in the event document: 
  https://static1.squarespace.com/static/68811cea22a48812262c6b93/t/689f9555ec63660c6cde75c9/1755288917300/LARC-OPEN-2025-Robot-Recolector-de-Cafe-Ver-1.0.pdf
- Summary of important constraints:
  - Timed runs: each match is time-limited. Score is based on items correctly collected and
    deposited within the time limit.
  - No external human control during the run.
  - Collisions or pushing items out of the allowed area may incur penalties or disqualification
    depending on severity (see official rules for precise penalties).
  - Items must be fully placed inside the robot to count as collected.
- For precise scoring (points per item type, bonuses, tie-break rules, and penalties) consult
  the linked PDF; this page only provides a concise summary and pointers.