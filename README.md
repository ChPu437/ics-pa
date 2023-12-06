# TO-DOs
* klib implemention
* infrastructure(2) -> ftrace
* device: [sound] dtrace

# Structure
- NEMU
  - monitor
    - sdb
  - cpu -> 抽象的CPU，独立于ISA
  - ISA
    - inst
    - intr -> 硬件异常
  - device -> 外设
- AM
  - trm -> 基本运算能力
  - klib -> 架构无关运行时库
  - ioe -> 外设接口(驱动)
  - cte -> 上下文管理
# ICS2022 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System)
in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to https://nju-projectn.github.io/ics-pa-gitbook/ics2022/

To initialize, run
```bash
bash init.sh subproject-name
```
See `init.sh` for more details.

The following subprojects/components are included. Some of them are not fully implemented.
* [NEMU](https://github.com/NJU-ProjectN/nemu)
* [Abstract-Machine](https://github.com/NJU-ProjectN/abstract-machine)
* [Nanos-lite](https://github.com/NJU-ProjectN/nanos-lite)
* [Navy-apps](https://github.com/NJU-ProjectN/navy-apps)
