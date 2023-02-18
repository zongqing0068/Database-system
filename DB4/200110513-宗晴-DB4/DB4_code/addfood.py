from PyQt5 import uic, QtWidgets

class Addfood:

    def __init__(self, db, cursor, username, ui):
        self.db = db
        self.cursor = cursor
        self.username = username
        self.old_ui = ui

        # 从文件中加载UI
        self.ui = uic.loadUi("addFood.ui")

        # 填充食物种类
        self.sql = 'select ft_id, ft_name from food_type order by ft_id;'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        self.food_type_len = len(self.data)
        self.ui.cbox_type.addItems(
            [str(self.data[i][0]) + ' ' + str(self.data[i][1]) for i in range(self.food_type_len)])

        # 填充生产厂家
        self.sql = 'select fm_id, fm_name from food_manufacturer order by fm_id;'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        self.food_manufacturer_len = len(self.data)
        self.ui.cbox_manufacturer.addItems(
            [str(self.data[i][0]) + ' ' + str(self.data[i][1]) for i in range(self.food_manufacturer_len)])

        self.ui.btn_type.clicked.connect(self.exec_type)
        self.ui.btn_manufacturer.clicked.connect(self.exec_manufacturer)
        self.ui.btn_add.clicked.connect(self.exec_add)

    def exec_type(self):
        type = self.ui.edit_type.text().strip()
        if type == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '种类不能为空！')
            return
        self.sql = 'select ft_id, ft_name from food_type where ft_name = \"' + type + '\";'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchone()
        if self.data is not None:
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '种类不能重复！')
            return

        self.sql = 'insert into food_type values (\"' + str(self.food_type_len + 1) + '\", \"' + type + '\");'
        # print(self.sql)
        self.cursor.execute(self.sql)
        self.db.commit()

        self.ui.cbox_type.addItem(str(self.food_type_len + 1) + ' ' + type)
        self.food_type_len = self.food_type_len + 1
        QtWidgets.QMessageBox.warning(self.ui, 'Success', '添加成功！')
        self.ui.edit_type.clear()

    def exec_manufacturer(self):
        manufacturer = self.ui.edit_manufacturer.text().strip()
        if manufacturer == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '厂家名称不能为空！')
            return
        self.sql = 'select fm_id, fm_name from food_manufacturer where fm_name = \"' + manufacturer + '\";'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchone()
        if self.data is not None:
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '厂家名称不能重复！')
            return

        self.sql = 'insert into food_manufacturer values (\"' + str(self.food_manufacturer_len + 1) + '\", \"' + manufacturer + '\");'
        # print(self.sql)
        self.cursor.execute(self.sql)
        self.db.commit()

        self.ui.cbox_manufacturer.addItem(str(self.food_manufacturer_len + 1) + ' ' + manufacturer)
        self.food_manufacturer_len = self.food_manufacturer_len + 1
        QtWidgets.QMessageBox.warning(self.ui, 'Success', '添加成功！')
        self.ui.edit_manufacturer.clear()

    def exec_add(self):
        name = self.ui.edit_name.text().strip()
        if name == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '请填写食物名称！')
            return

        type = self.ui.cbox_type.currentText()
        type_id = type.split(' ')[0]
        manufacturer = self.ui.cbox_manufacturer.currentText()
        manufacturer_id = manufacturer.split(' ')[0]

        self.sql = 'select count(*) from food;'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        new_food_id = str(int(self.data[0][0]) + 1)

        self.sql = 'insert into food values (\"' + new_food_id + '\", \"' + manufacturer_id + '\", \"' + \
                   type_id + '\", \"' + name + '\");'
        # print(self.sql)
        self.cursor.execute(self.sql)
        self.db.commit()

        self.old_ui.cbox_food.addItem(new_food_id + ' ' + name)
        QtWidgets.QMessageBox.warning(self.ui, 'Success', '添加成功！')
        self.ui.edit_name.clear()