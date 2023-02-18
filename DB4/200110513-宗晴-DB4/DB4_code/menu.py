from PyQt5 import uic, QtWidgets
from findcat import Findcat
from addcat import Addcat
from feedcat import Feedcat
from seecat import Seecat

class Menu:

    def __init__(self, db, cursor, username):
        self.db = db
        self.cursor = cursor
        self.username = username

        # 从文件中加载UI
        self.ui = uic.loadUi("menu.ui")

        self.ui.label_hello.setText('你好，' + username + '，欢迎登陆！')
        self.sql = 'select u_level from user where u_name = \"' + username + "\";"
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchone()
        self.level = self.data[0]
        self.ui.label_level.setText('当前等级：' + str(self.level))

        # 浏览/查询猫猫信息
        self.ui.btn_findcat.clicked.connect(self.exec_findcat)

        # 新增猫猫
        self.ui.btn_addcat.clicked.connect(self.exec_addcat)

        # 猫猫投喂登记
        self.ui.btn_feedcat.clicked.connect(self.exec_feedcat)

        # 猫猫出现位置打卡
        self.ui.btn_seecat.clicked.connect(self.exec_seecat)

    def exec_findcat(self):
        # 实例化并显示浏览/查询猫猫窗口
        self.ui_new = Findcat(self.db, self.cursor, self.username)
        self.ui_new.ui.show()

    def exec_addcat(self):
        # 实例化并显示浏览/查询猫猫窗口
        self.ui_new = Addcat(self.db, self.cursor, self.username)
        self.ui_new.ui.show()

    def exec_feedcat(self):
        # 实例化并显示浏览/查询猫猫窗口
        self.ui_new = Feedcat(self.db, self.cursor, self.username, self.ui)
        self.ui_new.ui.show()

    def exec_seecat(self):
        # 实例化并显示浏览/查询猫猫窗口
        self.ui_new = Seecat(self.db, self.cursor, self.username)
        self.ui_new.ui.show()
