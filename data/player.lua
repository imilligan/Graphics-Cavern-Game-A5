-- Ian Milligan 
-- CS 488 Assignment 5
-- The player structure
rootnode = gr.node('root')

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
skin = gr.material({0.8235, 0.6313, 0.5490}, {0.1, 0.1, 0.1}, 10)
chainmail = gr.texture('data/chainmail.png')
leather = gr.texture('data/leather.png')
face = gr.texture('data/face.png')
hair = gr.texture('data/hair.png')
wood = gr.texture('data/wood.png')
straw = gr.texture('data/straw.png')

-- Body
body = gr.sphere('body')
rootnode:add_child(body)
body:set_material(chainmail)
body:scale(2.0, 3.0, 1.6)

-- Top and bottom of body
top_of_body = gr.node('top_of_body')
rootnode:add_child(top_of_body)
top_of_body:translate(0, 2.75, 0)

bod_of_body = gr.node('bod_of_body')
rootnode:add_child(bod_of_body)
bod_of_body:translate(0, -2.9, 0)

-- Shoulders
shoulders_geo = gr.sphere('shoulders_geo')
top_of_body:add_child(shoulders_geo)
shoulders_geo:set_material(leather)
shoulders_geo:scale(3.125, 1.0, 1.0)

-- Hips
hips_geo = gr.sphere('hips_geo')
bod_of_body:add_child(hips_geo)
hips_geo:set_material(leather)
hips_geo:scale(2.0, 1.0, 1.0)

-- Left Arm
left_arm = gr.node('left_arm')
top_of_body:add_child(left_arm)
left_arm:rotate('x', 90)
left_arm:translate(-3.0, 0, 0)

-- Left Shoulder
left_shoulder_jt = gr.joint('left_shoulder_jt', {-135, 0, 90}, {0, 0, 0})
left_arm:add_child(left_shoulder_jt)

-- Left Bicep
left_bicep_geo = gr.sphere('left_bicep_geo')
left_shoulder_jt:add_child(left_bicep_geo)
left_bicep_geo:set_material(leather)
left_bicep_geo:translate(0, 0, 2.0)
left_bicep_geo:scale(0.5, 0.5, 2.0)

-- Left wrist + joint
left_wrist = gr.node('left_wrist')
left_shoulder_jt:add_child(left_wrist)
left_wrist:translate(0, 0, 3.5)

left_wrist_jt = gr.joint('left_wrist_jt', {-45, 0, 45}, {0, 0, 0})
left_wrist:add_child(left_wrist_jt)

-- Left Hand
left_hand_geo = gr.sphere('left_hand_geo')
left_wrist_jt:add_child(left_hand_geo)
left_hand_geo:set_material(skin)
left_hand_geo:translate(0, 0, 0.75)
left_hand_geo:scale(0.5, 0.5, 0.5)

-- Right Arm
right_arm = gr.node('right_arm')
top_of_body:add_child(right_arm)
right_arm:rotate('x', 90)
right_arm:translate(3.0, 0, 0)

-- Right Shoulder
right_shoulder_jt = gr.joint('right_shoulder_jt', {-225, 0, 90}, {0, 0, 0})
right_arm:add_child(right_shoulder_jt)

-- Right Bicep
right_bicep_geo = gr.sphere('right_bicep_geo')
right_shoulder_jt:add_child(right_bicep_geo)
right_bicep_geo:set_material(leather)
right_bicep_geo:translate(0, 0, 2.0)
right_bicep_geo:scale(0.5, 0.5, 2.0)

-- Right wrist + joint
right_wrist = gr.node('right_wrist')
right_shoulder_jt:add_child(right_wrist)
right_wrist:translate(0, 0, 3.5)

right_wrist_jt = gr.joint('right_wrist_jt', {-45, 0, 45}, {0, 0, 0})
right_wrist:add_child(right_wrist_jt)

-- Right Hand
right_hand_geo = gr.sphere('right_hand_geo')
right_wrist_jt:add_child(right_hand_geo)
right_hand_geo:set_material(skin)
right_hand_geo:translate(0, 0, 0.5)
right_hand_geo:scale(0.5, 0.5, 0.5)

broom = gr.node('broomstick')
right_wrist_jt:add_child(broom)

broom_geo = gr.sphere('broom_geo')
broom:add_child(broom_geo)
broom_geo:set_material(wood)
broom_geo:translate(0, 5, 0.5)
broom_geo:scale(0.25, 6.0, 0.25)

broom_head = gr.node('broom_head')
right_wrist_jt:add_child(broom_head)
broom_head:translate(0, 8, 0.25)

broom_head_geo = gr.box('broom_head_geo')
broom_head:add_child(broom_head_geo)
broom_head_geo:set_material(wood)
broom_head_geo:scale(3.0, 1.5, 0.5)
broom_head_geo:translate(-0.5, 0.0, 0.0)

broom_brush1_geo = gr.sphere('broom_brush1_geo')
broom_head:add_child(broom_brush1_geo)
broom_brush1_geo:set_material(straw)
broom_brush1_geo:scale(0.4, 1.0, 0.4)
broom_brush1_geo:translate(-2.5, 2.0, 0.75)

broom_brush2_geo = gr.sphere('broom_brush2_geo')
broom_head:add_child(broom_brush2_geo)
broom_brush2_geo:set_material(straw)
broom_brush2_geo:scale(0.4, 1.0, 0.4)
broom_brush2_geo:translate(-1.25, 2.0, 0.75)

broom_brush3_geo = gr.sphere('broom_brush3_geo')
broom_head:add_child(broom_brush3_geo)
broom_brush3_geo:set_material(straw)
broom_brush3_geo:scale(0.4, 1.0, 0.4)
broom_brush3_geo:translate(1.25, 2.0, 0.75)

broom_brush4_geo = gr.sphere('broom_brush4_geo')
broom_head:add_child(broom_brush4_geo)
broom_brush4_geo:set_material(straw)
broom_brush4_geo:scale(0.4, 1.0, 0.4)
broom_brush4_geo:translate(2.5, 2.0, 0.75)


-- Left Leg
left_leg = gr.node('left_leg')
bod_of_body:add_child(left_leg)
left_leg:rotate('x', 90)
left_leg:translate(-1.6, 0, 0)

-- Left Hip
left_hip_jt = gr.joint('left_hip_jt', {-90, 0, 45}, {0, 0, 0})
left_leg:add_child(left_hip_jt)

-- Left Thigh
left_thigh_geo = gr.sphere('left_thigh_geo')
left_hip_jt:add_child(left_thigh_geo)
left_thigh_geo:set_material(leather)
left_thigh_geo:translate(0, 0, 2.5)
left_thigh_geo:scale(0.65, 0.65, 2.5)

-- Left ankle + joint
left_ankle = gr.node('left_ankle')
left_hip_jt:add_child(left_ankle)
left_ankle:translate(0, 0, 4.4)

left_ankle_jt = gr.joint('left_ankle_jt', {-90, -90, -30}, {0, 0, 0})
left_ankle:add_child(left_ankle_jt)

-- Left Foot
left_foot_geo = gr.sphere('left_foot_geo')
left_ankle_jt:add_child(left_foot_geo)
left_foot_geo:set_material(blue)
left_foot_geo:translate(0, 0, 0.45)
left_foot_geo:scale(0.5, 0.5, 1.0)

-- Right Leg
right_leg = gr.node('right_leg')
bod_of_body:add_child(right_leg)
right_leg:rotate('x', 90)
right_leg:translate(1.6, 0, 0)

-- Right Hip
right_hip_jt = gr.joint('right_hip_jt', {-90, 0, 45}, {0, 0, 0})
right_leg:add_child(right_hip_jt)

-- Right Thigh
right_thigh_geo = gr.sphere('right_thigh_geo')
right_hip_jt:add_child(right_thigh_geo)
right_thigh_geo:set_material(leather)
right_thigh_geo:translate(0, 0, 2.5)
right_thigh_geo:scale(0.65, 0.65, 2.5)

-- Right ankle + joint
right_ankle = gr.node('right_ankle')
right_hip_jt:add_child(right_ankle)
right_ankle:translate(0, 0, 4.4)

right_ankle_jt = gr.joint('right_ankle_jt', {-90, -90, -30}, {0, 0, 0})
right_ankle:add_child(right_ankle_jt)

-- Right Foot
right_foot_geo = gr.sphere('right_foot_geo')
right_ankle_jt:add_child(right_foot_geo)
right_foot_geo:set_material(blue)
right_foot_geo:translate(0, 0, 0.45)
right_foot_geo:scale(0.5, 0.5, 1.0)

-- Collarbone
collarbone = gr.node('collarbone')
top_of_body:add_child(collarbone)
collarbone:translate(0, 0.75, 0)

-- Neck joint + geo
neck_jt = gr.joint('neck_jt', {-60, 0, 30}, {0, 0, 0})
collarbone:add_child(neck_jt)

neck_geo = gr.sphere('neck_geo')
neck_jt:add_child(neck_geo)
neck_geo:set_material(skin)
neck_geo:scale(0.5, 1.0, 0.5)
neck_geo:translate(0, 0.5, 0)

-- Upper neck + joint
upper_neck = gr.node('upper_neck')
neck_jt:add_child(upper_neck)
upper_neck:rotate('x', -90)
upper_neck:translate(0, 0, 1.0)

upper_neck_jt = gr.joint('upper_neck_jt', {-45, 0, 15}, {-75, 0, 75})
upper_neck:add_child(upper_neck_jt)

-- Head
head_geo = gr.sphere('head_geo')
upper_neck_jt:add_child(head_geo)
head_geo:set_material(face)
head_geo:translate(0, 0, 1.0)
head_geo:scale(1.25, 1.25, 1.4)
head_geo:rotate('y', 90)
head_geo:rotate('z', -15)

-- Hat
back_of_head = gr.node('top_of_head')
upper_neck_jt:add_child(back_of_head)
back_of_head:translate(0, 0.4, 1.45)

hat_geo = gr.sphere('hat_geo')
back_of_head:add_child(hat_geo)
hat_geo:set_material(hair)
hat_geo:scale(1.4, 1.1, 0.7)

rootnode:scale(0.125, 0.125, 0.125)


return rootnode
