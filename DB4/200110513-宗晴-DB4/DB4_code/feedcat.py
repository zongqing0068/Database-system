from PyQt5 import uic, QtWidgets
from PyQt5.QtCore import QDateTime

from addcat import Addcat
from addfood import Addfood


class Feedcat:

    def __init__(self, db, cursor, username, ui):
        self.db = db
        self.cursor = cursor
        self.username = username
        self.old_ui = ui

        # 从文件中加载UI
        self.ui = uic.loadUi("feedCat.ui")

        # 填充猫猫
        self.sql = 'select c_id, c_name from cat order by c_id;'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        self.cat_len = len(self.data)
        self.ui.cbox_cat.addItems(
            [str(self.data[i][0]) + ' ' + str(self.data[i][1]) for i in range(self.cat_len)])

        # 初始化时间
        self.ui.edit_time.setDateTime(QDateTime.currentDateTime())

        # 填充位置
        self.sql = 'select p_id, p_name from cat_position order by p_id;'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        self.cat_pos_len = len(self.data)
        self.ui.cbox_position.addItems(
            [str(self.data[i][0]) + ' ' + str(self.data[i][1]) for i in range(self.cat_pos_len)])

        # 填充食物
        self.sql = 'select f_id, f_name from food order by f_id;'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        self.cat_food_len = len(self.data)
        self.ui.cbox_food.addItems(
            [str(self.data[i][0]) + ' ' + str(self.data[i][1]) for i in range(self.cat_food_len)])

        self.ui.btn_position.clicked.connect(self.exec_position)
        self.ui.btn_food.clicked.connect(self.exec_food)
        self.ui.btn_add.clicked.connect(self.exec_add)

    def exec_position(self):
        position = self.ui.edit_position.text().strip()
        if position == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '地点不能为空！')
            return
        self.sql = 'select p_id, p_name from cat_position where p_name = \"' + position + '\";'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchone()
        if self.data is not None:
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '地点不能重复！')
            return

        self.sql = 'insert into cat_position values (\"' + str(self.cat_pos_len + 1) + '\", \"' + position + '\");'
        # print(self.sql)
        self.cursor.execute(self.sql)
        self.db.commit()

        self.ui.cbox_position.addItem(str(self.cat_pos_len + 1) + ' ' + position)
        self.cat_pos_len = self.cat_pos_len + 1
        QtWidgets.QMessageBox.warning(self.ui, 'Success', '添加成功！')
        self.ui.edit_position.clear()

    def exec_food(self):
        self.ui_new = Addfood(self.db, self.cursor, self.data[0], self.ui)
        self.ui_new.ui.show()

    def exec_add(self):
        cat = self.ui.cbox_cat.currentText()
        cat_id = cat.split(' ')[0]
        time = self.ui.edit_time.dateTime().toString("yyyy-MM-dd HH:mm:ss")
        # print(time)
        position = self.ui.cbox_position.currentText()
        position_id = position.split(' ')[0]
        food = self.ui.cbox_food.currentText()
        food_id = food.split(' ')[0]

        self.sql = 'insert into feed values (\"' + cat_id + '\", \"' + self.username + '\", \"' + \
                   food_id + '\", \"' + position_id + '\", \"' + time + '\");'
        # print(self.sql)
        self.cursor.execute(self.sql)
        self.db.commit()

        QtWidgets.QMessageBox.warning(self.ui, 'Success', '添加成功！')

        self.sql = 'select u_level from user where u_name = \"' + self.username + "\";"
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchone()
        self.level = self.data[0]
        self.old_ui.label_level.setText('当前等级：' + str(self.level))
