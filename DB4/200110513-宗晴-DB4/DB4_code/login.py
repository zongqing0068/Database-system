import sys
from PyQt5 import uic, QtWidgets
from register import Register
from menu import Menu

class Login:

    def __init__(self, db, cursor):
        self.db = db
        self.cursor = cursor

        # 从文件中加载UI
        self.ui = uic.loadUi("login.ui")

        # 登录
        self.ui.btn_login.clicked.connect(self.exec_login)
        self.ui.edit_password.returnPressed.connect(self.exec_login)

        # 注册
        self.ui.btn_register.clicked.connect(self.exec_register)

    def exec_login(self):
        username = self.ui.edit_username.text().strip()
        password = self.ui.edit_password.text().strip()
        if username == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '用户名不能为空！')
            return
        elif password == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '密码不能为空！')
            return

        self.sql = 'select u_name, u_password from user where u_name = \"' + username + "\";"
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchone()
        if self.data is None:
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '不存在该用户名，请先注册！')
            self.ui.edit_username.clear()
            self.ui.edit_password.clear()
            return
        elif self.data[1] == password:
            # print("success")
            self.ui_new = Menu(self.db, self.cursor, self.data[0])
            self.ui_new.ui.show()
            self.ui.close()
        else:
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '密码错误！')
            return

    def exec_register(self):
        # 实例化并显示注册窗口
        self.ui_new = Register(self.db, self.cursor)
        self.ui_new.ui.show()


def showLoginUI(db, cursor):
    app = QtWidgets.QApplication(sys.argv)
    loginUI = Login(db, cursor)
    loginUI.ui.show()
    sys.exit(app.exec_())
