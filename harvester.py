#!/usr/bin/python2.7

import sqlite3, traceback,os

class Harvester:

    def __init__(self, create_new = False, quiet = False):
        self.create_new = create_new
        self.quiet = quiet
        self.conn = None
        self.cursor = None

        self.last_app_id = 0
        self.last_permission_id = 0

        if self.create_new:
            if os.path.exists("BDDHarvester.sq3"):
                os.remove("BDDHarvester.sq3")
        #init connection to database
                self.init_connection()
                os.chmod("BDDHarvester.sq3", 0o666)
            else:
                self.init_connection()
            self.create_tables()
        else:
            self.init_connection()
        
    def init_connection(self):
        self.conn = sqlite3.connect("BDDHarvester.sq3")
        self.cursor = self.conn.cursor()

    def close_connection(self):
        self.cursor.close()
        self.conn.close()

    def create_database(self):
        #this method can create a database, if needed
        #db = mdb.connect(host="localhost", user="root", passwd="1234")
        #cursor = db.cursor()
        try:
            sql = "CREATE DATABASE harvester"
            cursor.execute(sql)
            db.commit()
            cursor.close()
            db.close()
            return True
        except:
            print "Error creating database"
            return False

    def create_tables(self):
        #this method will take care of creating all the tables we need

        table_applications = """CREATE TABLE Applications (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            size varchar(100)    NOT NULL ,
            constructor varchar(100)    NOT NULL ,
            sharedUserID varchar(100)    NOT NULL ,
            file_name varchar(100)    NOT NULL,
            has_dynamic_code int,
            score int
        );"""

        table_permissions = """CREATE TABLE Permissions (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            name varchar(100)    NOT NULL 
        );"""

        table_usages = """CREATE TABLE Usages (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            appID int    NOT NULL ,
            permissionID int    NOT NULL ,
            smali text    NOT NULL ,
            java text    NOT NULL,
            FOREIGN KEY(appID) REFERENCES Applications(ID),
            FOREIGN KEY(permissionID) REFERENCES Permissions(ID)
        );"""

        table_dynamic_code = """CREATE TABLE Dynamic (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            appID int NOT NUL,
            smali text NOT NULL,
            FOREIGN KEY(appID) REFERENCES Applications(ID)
        );"""
        #ref_apps_usages = """ALTER TABLE Usages ADD CONSTRAINT APPLICATIONS_USAGES FOREIGN KEY APPLICATIONS_USAGES (appID)
        #REFERENCES Applications (ID);"""
        
        #ref_permis_usages = """ALTER TABLE Usages ADD CONSTRAINT PERMISSIONS_USAGES FOREIGN KEY PERMISSIONS_USAGES (permissionID)
        #REFERENCES Permissions (ID);"""

        try:
            self.cursor.execute(table_applications)
            self.cursor.execute(table_permissions)
            self.cursor.execute(table_usages)
            
            #self.cursor.execute(ref_apps_usages)
            #self.cursor.execute(ref_permis_usages)

            self.conn.commit()
        except Exception,e:
            print "Error creating tables"
            print str(e)
            traceback.print_exc()
    
    def get_last_app_id(self):
            return self.last_app_id

    def get_last_permission_id(self):
            return self.last_permission_id
    
    def add_app(self, filename, size, constructor, sharedUserID, has_dynamic_code, score):
        insert_app = "INSERT INTO Applications(file_name, constructor, size, sharedUserID) VALUES(?,?,?,?)"
        try:
            self.cursor.execute(insert_app,(filename, size, constructor, sharedUserID)) 
            self.conn.commit()

            self.last_app_id = self.cursor.lastrowid

        except:
            print "Error inserting app data into database"
            traceback.print_exc()

    def add_permission(self, name):
        insert_permission = "INSERT INTO Permissions (name) VALUES ('%s')" % (name)
        try:
            self.cursor.execute(insert_permission)
            self.conn.commit()

            self.last_permission_id = self.cursor.lastrowid

        except:
            print "Error inserting given permission name into database"
    
    def add_usage(self, appID, permissionID, smali_code, java_code):
        insert_code = "INSERT INTO Usages (appID, permissionID, smali, java) VALUES ('%s', '%s', '%s', '%s')" % (appID, permissionID, smali_code, java_code)
        try:
            self.cursor.execute(insert_code)
            self.conn.commit()
        except:
            print "Error inserting source code into database"

    def add_dynamic_code(self, appID, smali):
        insert_dynamic_code = "INSERT INTO Dynamic (appID, smali) VALUES (?,?)"
        try:
            self.cursor.execute(insert_dynamic_code, (appID, smali))
            self.conn.commit()

        except:
            print "Error inserting dynamic code into database"
            traceback.print_exc()

