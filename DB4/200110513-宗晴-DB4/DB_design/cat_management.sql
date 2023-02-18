/*==============================================================*/
/* DBMS name:      MySQL 5.0                                    */
/* Created on:     2022/12/27 14:50:43                          */
/*==============================================================*/


-- drop trigger update_user_level;

-- drop trigger update_cat_position;

-- drop 
-- table if exists viewFoodInfo;

-- drop 
-- table if exists viewCatInfo;

-- drop index idx_user_level on user;

create database cat_management;

use cat_management;

/*==============================================================*/
/* Table: cat                                                   */
/*==============================================================*/
create table cat
(
   c_id                 int not null,
   p_id                 int not null,
   ct_id                int not null,
   c_name               char(10) not null,
   c_colour             char(10) not null,
   c_age                int,
   c_personality        char(40),
   primary key (c_id)
);

/*==============================================================*/
/* Table: cat_type                                              */
/*==============================================================*/
create table cat_type
(
   ct_id                int not null,
   ct_name              char(20) not null,
   primary key (ct_id)
);

/*==============================================================*/
/* Table: feed                                                  */
/*==============================================================*/
create table feed
(
   c_id                 int not null,
   u_name               char(10) not null,
   f_id                 int not null,
   p_id                 int not null,
   fe_time              datetime not null,
   primary key (c_id, u_name, f_id, p_id, fe_time)
);

/*==============================================================*/
/* Table: food                                                  */
/*==============================================================*/
create table food
(
   f_id                 int not null,
   fm_id                int not null,
   ft_id                int not null,
   f_name               char(30) not null,
   primary key (f_id)
);

/*==============================================================*/
/* Table: food_manufacturer                                     */
/*==============================================================*/
create table food_manufacturer
(
   fm_id                int not null,
   fm_name              char(30) not null,
   primary key (fm_id)
);

/*==============================================================*/
/* Table: food_type                                             */
/*==============================================================*/
create table food_type
(
   ft_id                int not null,
   ft_name              char(10) not null,
   primary key (ft_id)
);

/*==============================================================*/
/* Table: cat_position                                              */
/*==============================================================*/
create table cat_position
(
   p_id                 int not null,
   p_name               char(20) not null,
   primary key (p_id)
);

/*==============================================================*/
/* Table: position_punch                                        */
/*==============================================================*/
create table position_punch
(
   c_id                 int not null,
   u_name               char(10) not null,
   p_id                 int not null,
   pp_time              datetime not null,
   primary key (c_id, u_name, p_id, pp_time)
);

/*==============================================================*/
/* Table: user                                                  */
/*==============================================================*/
create table user
(
   u_name               char(10) not null,
   u_password           char(10) not null,
   u_phone              numeric(11,0) not null,
   u_gender             char(1) not null,
   u_age                int not null,
   u_level              int not null,
   primary key (u_name)
);

/*==============================================================*/
/* Index: idx_user_level                                        */
/*==============================================================*/
create index idx_user_level on user
(
   u_level
);

/*==============================================================*/
/* Index: idx_cat_name                                        */
/*==============================================================*/
create index idx_cat_name on cat
(
   c_name
);

/*==============================================================*/
/* View: viewCatInfo                                            */
/*==============================================================*/
create VIEW  viewCatInfo
 as
select cat.c_id, cat.c_name, cat.c_colour, cat.c_age, 
cat.c_personality, cat_type.ct_name, cat_position.p_name
from cat, cat_type, cat_position
where cat.ct_id = cat_type.ct_id
and cat.p_id = cat_position.p_id
order by cat.c_id;

/*==============================================================*/
/* View: viewFoodInfo                                           */
/*==============================================================*/
create VIEW  viewFoodInfo
 as
select food.f_id, food.f_name, food_type.ft_name, food_manufacturer.fm_name
from food, food_type, food_manufacturer
where food.ft_id = food_type.ft_id
and food.fm_id = food_manufacturer.fm_id
order by food.f_id;

alter table cat add constraint FK_cat_cattype foreign key (ct_id)
      references cat_type (ct_id) on delete restrict on update restrict;

alter table cat add constraint FK_cat_position foreign key (p_id)
      references cat_position (p_id) on delete restrict on update restrict;

alter table feed add constraint FK_feed foreign key (p_id)
      references cat_position (p_id) on delete restrict on update restrict;

alter table feed add constraint FK_feed2 foreign key (c_id)
      references cat (c_id) on delete restrict on update restrict;

alter table feed add constraint FK_feed3 foreign key (u_name)
      references user (u_name) on delete restrict on update restrict;

alter table feed add constraint FK_feed4 foreign key (f_id)
      references food (f_id) on delete restrict on update restrict;

alter table food add constraint FK_foodmanufacturer_food foreign key (fm_id)
      references food_manufacturer (fm_id) on delete restrict on update restrict;

alter table food add constraint FK_foodtype_food foreign key (ft_id)
      references food_type (ft_id) on delete restrict on update restrict;

alter table position_punch add constraint FK_position_punch foreign key (p_id)
      references cat_position (p_id) on delete restrict on update restrict;

alter table position_punch add constraint FK_position_punch2 foreign key (c_id)
      references cat (c_id) on delete restrict on update restrict;

alter table position_punch add constraint FK_position_punch3 foreign key (u_name)
      references user (u_name) on delete restrict on update restrict;


DELIMITER ;;
CREATE TRIGGER update_user_level
    AFTER INSERT ON feed
    FOR EACH ROW
BEGIN
update user
set u_level = u_level + 1
where user.u_name = new.u_name;
END;;


CREATE TRIGGER update_cat_position
    AFTER INSERT ON position_punch
    FOR EACH ROW
BEGIN
update cat
set cat.p_id = (select distinct p_id
                from position_punch
                where c_id = new.c_id
                group by p_id
                order by count(*) desc
                limit 1)
where cat.c_id = new.c_id;
END;;
DELIMITER ;

CREATE TRIGGER initialize_user_level
    BEFORE INSERT ON user
    FOR EACH ROW
SET new.u_level = 0;

insert into cat_type(ct_id, ct_name)
values(1, "橘猫"), (2, "奶牛猫"), (3, "玳瑁猫"), (4, "狸花猫");

insert into cat_position(p_id, p_name)
values(1, "荔园6、7栋之间楼梯"), (2, "荔园3食堂对面"), (3, "A栋、G栋之间"), 
(4, "荔园6栋楼下"), (5, "荔园一食堂门口");

insert into cat(c_id, p_id, ct_id, c_name, c_colour, c_age, c_personality)
values(1, 5, 3, "小垃圾", "黑色+橘色", 3, "爱翻垃圾桶，怕人"),
(2, 2, 1, "橘白", "橘色+白色", 5, "贪吃，很凶"),
(3, 2, 2, "肥黑", "黑色+白色", 4, "黏人可rua"),
(4, 2, 1, "小橘", "橘色", 3, "黏人"),
(5, 1, 1, "小小橘", "橘色", 1, "黏人爱贴贴"),
(6, 4, 1, "猫哥", "橘色+白色", 5, "较高冷"),
(7, 2, 1, "小乖", "橘色+白色", 2, "亲人，贪吃"),
(8, 3, 4, "烤鸭", "黄里透灰", 6, "高冷，大学城大姐大");

insert into food_manufacturer(fm_id, fm_name)
values(1, "菏泽开饭乐宠物食品科技有限公司"), (2, "山东汉欧生物科技有限公司"), 
(3, "辽宁海辰宠物有机食品有限公司"), (4, "山东烟台中宠食品股份有限公司");

insert into food_type(ft_id, ft_name)
values(1, "主食"), (2, "零食");

insert into food(f_id, fm_id, ft_id, f_name)
values(1, 1, 1, "凯锐思成猫专用牛肉猫粮"), (2, 1, 1, "凯锐思成猫专用鱼肉猫粮"),
 (3, 1, 1, "凯锐思幼猫猫粮奶糕鱼肉猫粮"), (4, 2, 1, "网易严选全价冻干双拼"),
 (5, 2, 2, "猫用浓汤红肉罐头"), (6, 2, 2, "猫用金枪鱼鸡肉罐头"),
 (7, 2, 2, "猫用鸡肉三文鱼罐头"), (8, 2, 1, "凯锐思金枪鱼肉条"),
 (9, 2, 1, "凯锐思鸡肉条");
