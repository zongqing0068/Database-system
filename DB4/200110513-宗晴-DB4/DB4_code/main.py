import pymysql
from login import showLoginUI


if __name__ == '__main__':
    db = pymysql.connect(host="localhost", user="root", password="hellomysql", database="cat_management")
    cursor = db.cursor()

    showLoginUI(db, cursor)

