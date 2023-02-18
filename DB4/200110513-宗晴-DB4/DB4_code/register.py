import re
from PyQt5 import uic, QtWidgets

class Register:

    def __init__(self, db, cursor):
        self.db = db
        self.cursor = cursor

        # 从文件中加载UI
        self.ui = uic.loadUi("register.ui")

        # 注册
        self.ui.btn_register.clicked.connect(self.exec_register)



    def exec_register(self):
        # 获取用户名
        username = self.ui.edit_username.text().strip()
        if username == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '请填写用户名！')
            return
        self.sql = 'select u_name from user where u_name = \"' + username + "\";"
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchone()
        if self.data is not None:
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '用户名已存在！')
            return

        # 获取密码
        password = self.ui.edit_password.text().strip()
        if password == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '请填写密码！')
            return
        if len(password) < 6:
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '密码至少为6位！')
            return
        password2 = self.ui.edit_password2.text().strip()
        if password2 == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '请再次填写密码！')
            return
        if password2 != password:
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '两次密码不同！')
            return

        # 获取性别
        gender = self.ui.cbox_gender.currentText()

        # 获取年龄
        age = self.ui.edit_age.text().strip()
        if not age.isdigit():
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '年龄必须为数字！')
            return

        # 获取电话
        phone = self.ui.edit_phone.text().strip()
        if phone == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '请填写电话！')
            return
        if re.match('^[0-9]{11}$', phone) is None:
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '电话为11位数字！')
            return

        self.sql = 'insert into user(u_name, u_password, u_phone, u_gender, u_age) values (\"' \
                   + username + '\", \"' + password + '\", \"' + phone + '\", \"' + gender + '\", \"' + age + '\")'
        # print(self.sql)
        self.cursor.execute(self.sql)
        self.db.commit()

        QtWidgets.QMessageBox.warning(self.ui, 'Success', '注册成功！')

        self.sql = 'select u_name, u_password, u_phone, u_gender, u_age, u_level from user'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        # print(self.data)

        # 关闭自己
        self.ui.close()