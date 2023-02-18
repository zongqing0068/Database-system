from PyQt5 import uic, QtWidgets
from PyQt5.QtCore import QDateTime

from addcat import Addcat

class Seecat:

    def __init__(self, db, cursor, username):
        self.db = db
        self.cursor = cursor
        self.username = username

        # 从文件中加载UI
        self.ui = uic.loadUi("seeCat.ui")

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

        self.ui.btn_position.clicked.connect(self.exec_position)
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

    def exec_add(self):
        cat = self.ui.cbox_cat.currentText()
        cat_id = cat.split(' ')[0]
        time = self.ui.edit_time.dateTime().toString("yyyy-MM-dd HH:mm:ss")
        # print(time)
        position = self.ui.cbox_position.currentText()
        position_id = position.split(' ')[0]
        self.sql = 'insert into position_punch values (\"' + cat_id + '\", \"' + self.username + '\", \"' + \
                   position_id + '\", \"' + time + '\");'
        # print(self.sql)
        self.cursor.execute(self.sql)
        self.db.commit()

        QtWidgets.QMessageBox.warning(self.ui, 'Success', '添加成功！')

