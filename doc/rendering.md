```mermaid
gantt
    title Rendering 60fps
    dateFormat  SSS
    axisFormat %Lms
    todayMarker on


    v-blank 1: milestone, m1, 160, 0
    on-present: milestone, m2, 160,0
    on-paint: milestone, m4, 170, 0
    v-blank 2: milestone, m3, 176, 0
    section Output
    update tree 6ms: o1, 154, 160
    paint 10ms:o2, 160, 170
    GL present 6ms:after o2, 170, 176
    update tree 6ms: o3, 170, 176
```