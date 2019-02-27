from utils import get_connection

c = get_connection()
curs = c.cursor()

def print_table(table):
    print("--------------%s----------------" % table)
    curs.execute("SELECT * FROM %s" % table)
    for row in curs:
        print(row)


print_table("VENDINGMACHINE")
print_table("PRODUCTS")
print_table("CUSTOMERS")
print_table("OWNERS")
print_table("STOCK")

