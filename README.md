# 2023工训主控代码

#### 介绍
  2023年工程能力训练大赛 黑龙江省赛区 哈尔滨商业大学
  黑龙江省第三名 国赛决赛第四十八名
  国赛决赛裁判组认为我们爪子设计不符合规则规范 扣除了决赛50%分数 遗憾离场
  注意：以后进入国赛的朋友们 记得手爪部分是包括爪上舵机齿轮部分也算作手爪 也就是运动的部分都统称为手爪 一定要记得决赛要将这部分完全替换
  否则可能会出现想我们一样被认定不符合规则从而被扣除积分

#### 软件架构
1.  主控：Arduino Mega2560 Pro
2.  视觉：K210 bit
3.  电机选择：张大头42闭环步进
4.  电机控制：总线TTL控制
5.  舵机选择：飞特舵机
6.  舵机控制：总线TTL控制
7.  陀螺仪：维特智能HWT101CT
8.  麦轮：天府之土
9.  激光雷达：北醒TFminiPlus（还不错？）
10. 编程平台：Visual Studio Code Platformio、Maixpyide

#### 使用说明
1.  xxxx
2.  xxxx
3.  xxxx

#### 改进建议
1.  麦轮：建议选择全向轮或轴承辊间隔更小的麦轮，如大疆麦轮等，震动更小陀螺仪漂移更低
2.  主控：Arduino平台的大部分控制板性能太差 主频较低 且上传程序速度过慢 我后期发现总线控制的频率会无法上去原因便在此
3.  视觉：建议上树莓派、Jetson、瑞芯微等作为视觉处理模块和上位机，单独的K210或OPENMV高性能版本也可以做到大部分任务

#### 参与贡献
哈尔滨商业大学 端点科技工作室 燦
