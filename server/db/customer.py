class Customer():
    def __init__(self, id, dl_id, email, address, name, credits, phone_number, password, cardnumber):
        self.id = id
        self.dl_id = dl_id
        self.email = email
        self.address = address
        self.name = name
        self.credits = credits
        self.phone_number = phone_number
        self.password = password
        self.cardnumber = cardnumber


def insertCustomer(db_conn, ID, EMAIL, ADDRESS, PHONENUM, NAME, CREDITS, PASSWORD, CARDNUMBER):
    sql = "INSERT INTO CUSTOMERS (DL_ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUMBER, USERPASS, CARDNUMBER) VALUES(%s, %s, %s, %s, %s, %s, %s, %s)"
    val = (ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUM, PASSWORD, CARDNUMBER)
    curs = db_conn.cursor()
    curs.execute(sql, val)
    db_conn.commit()
    print(curs.rowcount, "record inserted.")

def getDate(db_conn, DL_ID):
    sql = "select DATEOFBIRTH FROM DRIVERS"
    val = (DL_ID)
    print(DL_ID)
    curs = db_conn.cursor()
    curs.execute(sql)
    print("hither##############################################################")
    for row in curs:
        date = row
    return date    

def changeName(db_conn, NAME, ID):
    sql = "UPDATE CUSTOMERS SET NAME=%s WHERE ID = %s"
    val = (NAME, ID)
    curs = db_conn.cursor()
    curs.execute(sql, val)
    db_conn.commit()


def changeEmail(db_conn, EMAIL, ID):
    sql = "UPDATE CUSTOMERS SET EMAIL=%s WHERE ID = %s"
    val = (EMAIL, ID)
    curs = db_conn.cursor()
    curs.execute(sql, val)
    db_conn.commit()


def changeDL(db_conn, DL_ID, ID):
    sql = "UPDATE CUSTOMERS SET DL_ID=%s WHERE ID = %s"
    val = (DL_ID, ID)
    curs = db_conn.cursor()
    curs.execute(sql, val)
    db_conn.commit()


def changeAddress(db_conn, ADDRESS, ID):
    sql = "UPDATE CUSTOMERS SET ADDRESS=%s WHERE ID = %s"
    val = (ADDRESS, ID)
    curs = db_conn.cursor()
    curs.execute(sql, val)
    db_conn.commit()


def changePhone(db_conn, PHONE, ID):
    sql = "UPDATE CUSTOMERS SET PHONENUMBER=%s WHERE ID = %s"
    val = (PHONE, ID)
    curs = db_conn.cursor()
    curs.execute(sql, val)
    db_conn.commit()


def changePass(db_conn, PASSWORD, ID):
    sql = "UPDATE CUSTOMERS SET USERPASS=%s WHERE ID = %s"
    val = (PASSWORD, ID)
    curs = db_conn.cursor()
    curs.execute(sql, val)
    db_conn.commit()


def changeCard(db_conn, CARDNUMBER, ID):
    sql = "UPDATE CUSTOMERS SET CARDNUMBER=%s WHERE ID = %s"
    val = (CARDNUMBER, ID)
    curs = db_conn.cursor()
    curs.execute(sql, val)
    db_conn.commit()

def changeCredits(db_conn, CREDITS, ID):
    sql = "UPDATE CUSTOMERS SET CREDITS=%s WHERE ID = %s"
    val = (CREDITS, ID)
    curs = db_conn.cursor()
    curs.execute(sql, val)
    db_conn.commit()

def getCustomerByBarcode(db_conn, barcode_id):
    query = """SELECT ID, DL_ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUMBER, USERPASS, CARDNUMBER
                   FROM CUSTOMERS WHERE DL_ID=%s"""
    curs = db_conn.cursor()
    curs.execute(query, [barcode_id])
    for row in curs:
        return Customer(*row)

def getCredits(db_conn, id):
    query = """SELECT CREDITS
        FROM CUSTOMERS WHERE ID=%s"""
    curs = db_conn.cursor()
    curs.execute(query, [id])
    for row in curs:
        return row[0]

def getCustomerByEmail(db_conn, email):
    query = """SELECT ID, DL_ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUMBER, USERPASS, CARDNUMBER
                   FROM CUSTOMERS WHERE EMAIL=%s"""
    curs = db_conn.cursor()
    curs.execute(query, [email])
    for row in curs:
        return Customer(*row)

def getCustomerByID(db_conn, id):
    query = """SELECT ID, DL_ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUMBER, USERPASS, CARDNUMBER
                   FROM CUSTOMERS WHERE ID=%s"""
    curs = db_conn.cursor()
    curs.execute(query, [id])
    for row in curs:
         return Customer(*row)
        
def updateCredits(db_conn, id, newValue):
    print(newValue)
    query = """UPDATE CUSTOMERS SET CREDITS = %s WHERE ID = %s"""
    curs = db_conn.cursor()
    curs.execute(query, [newValue, id])
    db_conn.commit()

