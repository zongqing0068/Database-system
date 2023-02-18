from PyQt5 import uic, QtWidgets
from PyQt5.QtCore import Qt

class Findcat:

    def __init__(self, db, cursor, username):
        self.db = db
        self.cursor = cursor
        self.username = username

        # 从文件中加载UI
        self.ui = uic.loadUi("findCat.ui")
        self.ui.tbl_cat.setColumnWidth(0, 40)
        self.ui.tbl_cat.setColumnWidth(1, 80)
        self.ui.tbl_cat.setColumnWidth(2, 120)
        self.ui.tbl_cat.setColumnWidth(3, 40)
        self.ui.tbl_cat.setColumnWidth(4, 160)
        self.ui.tbl_cat.setColumnWidth(5, 80)
        self.ui.tbl_cat.setColumnWidth(6, 160)

        self.ui.btn_findcat.clicked.connect(self.exec_findcat)
        self.ui.btn_allcat.clicked.connect(self.exec_allcat)


    def exec_findcat(self):
        cat_name = self.ui.edit_catname.text().strip()
        if cat_name == '':
            QtWidgets.QMessageBox.warning(self.ui, 'Warning', '请输入猫猫昵称！')
            return
        self.sql = 'select * from viewcatinfo where c_name = \"' + cat_name + "\";"
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()

        self.row_num = len(self.data)
        self.ui.tbl_cat.setRowCount(self.row_num)
        for i in range(self.row_num):
            for j in range(len(self.data[0])):
                item = QtWidgets.QTableWidgetItem()
                item.setText(str(self.data[i][j]))
                item.setTextAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
                self.ui.tbl_cat.setItem(i, j, item)

    def exec_allcat(self):
        self.sql = 'select * from viewcatinfo;'
        self.cursor.execute(self.sql)
        self.data = self.cursor.fetchall()
        # print(list(self.data))

        self.row_num = len(self.data)
        self.ui.tbl_cat.setRowCount(self.row_num)
        for i in range(self.row_num):
            for j in range(len(self.data[0])):
                item = QtWidgets.QTableWidgetItem()
                item.setText(str(self.data[i][j]))
                item.setTextAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
                self.ui.tbl_cat.setItem(i, j, item)


