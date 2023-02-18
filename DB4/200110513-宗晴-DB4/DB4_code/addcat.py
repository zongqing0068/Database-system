from PyQt5 import uic, QtWidgets

class Addcat:

    def __init__(self, db, cursor, username):
        self.db = db
        self.cursor = cursor
        self.username = username

        # 从文件中加载UI
        self.ui = uic.loadUi("addCat.ui")

        # 填充猫猫种类
        self.sql = 'select ct_id, ct_name from cat_type order by ct_id;'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        self.cat_type_len = len(self.data)
        self.ui.cbox_type.addItems([str(self.data[i][0]) + ' ' + str(self.data[i][1]) for i in range(self.cat_type_len)])

        # 填充位置
        self.sql = 'select p_id, p_name from cat_position order by p_id;'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        self.cat_pos_len = len(self.data)
        self.ui.cbox_position.addItems([str(self.data[i][0]) + ' ' + str(self.data[i][1]) for i in range(self.cat_pos_len)])

        self.ui.btn_type.clicked.connect(self.exec_type)
        self.ui.btn_position.clicked.connect(self.exec_position)
        self.ui.btn_addcat.clicked.connect(self.exec_addcat)

    def exec_type(self):
        type = self.ui.edit_type.text().strip()
        if type == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '种类不能为空！')
            return
        self.sql = 'select ct_id, ct_name from cat_type where ct_name = \"' + type + '\";'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchone()
        if self.data is not None:
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '种类不能重复！')
            return

        self.sql = 'insert into cat_type values (\"' + str(self.cat_type_len + 1) + '\", \"' + type + '\");'
        # print(self.sql)
        self.cursor.execute(self.sql)
        self.db.commit()

        self.ui.cbox_type.addItem(str(self.cat_type_len + 1) + ' ' + type)
        self.cat_type_len = self.cat_type_len + 1
        QtWidgets.QMessageBox.warning(self.ui, 'Success', '添加成功！')
        self.ui.edit_type.clear()

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

    def exec_addcat(self):
        name = self.ui.edit_name.text().strip()
        if name == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '请填写猫猫昵称！')
            return

        colour = self.ui.edit_colour.text().strip()
        if colour == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '请填写猫猫颜色！')
            return

        age = self.ui.edit_age.text().strip()
        if age == '':
            age = "0"
        personality = self.ui.edit_personality.text().strip()
        type = self.ui.cbox_type.currentText()
        type_id = type.split(' ')[0]
        position = self.ui.cbox_position.currentText()
        position_id = position.split(' ')[0]

        self.sql = 'select count(*) from cat;'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        new_cat_id = str(int(self.data[0][0]) + 1)

        self.sql = 'insert into cat values (\"' + new_cat_id + '\", \"' + position_id + '\", \"' + \
                   type_id + '\", \"' + name + '\", \"' + colour + '\", \"' + age + '\", \"' + personality + '\");'
        # print(self.sql)
        self.cursor.execute(self.sql)
        self.db.commit()

        QtWidgets.QMessageBox.warning(self.ui, 'Success', '添加成功！')
        self.ui.edit_name.clear()
        self.ui.edit_colour.clear()
        self.ui.edit_age.clear()
        self.ui.edit_personality.clear()

