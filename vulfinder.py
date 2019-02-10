#!/usr/bin/python2.7

import argparse,sys

from androguard.core.bytecodes import apk, dvm
from androguard.core.analysis import analysis,ganalysis

from androguard.decompiler.dad import decompile

from harvester import *

class VulnerabilityFinder:

    def __init__(self, filename, quiet = False):
        
        self.filename = filename
        self.quiet = quiet
        self.application = None
        self.dalvik_executable = None
        self.analyzed_dex = None
        self.gx = None
        
        self.permissions = None
        self.methods = []
        
        #stores the data that we will return to caller
        self.source_code = {}
        self.smali = []
        self.java = []

    def get_type(self, filename):
        
        extension = os.path.splitext(filename)[1]

        if extension in list((".odex", ".dex", ".apk")):
            return extension

        else:
            return "invalid"

    def parse_input_file(self):
        """This function will analyze the file given as input"""

        extension = self.get_type(self.filename)

        if extension == ".apk":
            if not self.quiet:
                print "[*] Opening APK..."

            try:
                self.application = apk.APK(self.filename)
                self.dalvik_executable = dvm.DalvikVMFormat(self.application.get_dex())
            except:
                return None
        
        elif extension == ".odex":
            if not self.quiet:
                print "[*] Opening .odex file..."
                
            self.dalvik_executable = dvm.DalvikOdexVMFormat(open(self.filename, "r").read())
        
        elif extension == ".dex":
            if not self.quiet:
                print "[*] Opening .dex file"

            self.dalvik_executable = dvm.DalvikVMFormat(open(self.filename, "r").read())
        
        else:

            if not self.quiet:
                print "[!] Invalid type for %s...skipping" % (self.filename)

            return None
        
        if not self.quiet:
            #print "[*] This application uses the following permissions :"
            #print self.application.get_permissions()

            print "[*] Analyzing dex file..."

        self.analyzed_dex = analysis.VMAnalysis(self.dalvik_executable)
        self.gx = ganalysis.GVMAnalysis(self.analyzed_dex, None)

        self.dalvik_executable.set_vmanalysis(self.analyzed_dex)
        self.dalvik_executable.set_gvmanalysis(self.gx)

        if not self.quiet:
            print "[*] Creating xrefs..."

        self.dalvik_executable.create_xref()
        self.dalvik_executable.create_python_export()

    def get_permissions(self):
        #Parse the AndroidManifest.xml file to find permissions.

        #contains the refs to the usages of the permissions
        try:
            self.permissions = self.analyzed_dex.get_permissions([])
        except:
            return None
        
        #human readable
        permissions = list(self.permissions.keys())
        return permissions #human

    def get_permission_code(self, permission):
        """For a given permission, this function will find every instruction that needed to be allowed by it. Eventually, the corresponding source code for each method will be fetched."""
        
        #reset
        self.smali = []
        self.java = []
        self.source_code = {}

        try:
            paths = self.permissions[permission.upper()]
        except KeyError:
            print self.permissions
            return None
        
        if not self.quiet:
            print "[*] The permission %s was necessary for the following instructions :" % (permission)
        
        for path in paths:
            self.show_path(path)
            #debug : remove the instruction below after debug phase.
            break

        #now that we got the paths instances, let's get the source code
        self.decompile_methods()

        self.source_code['smali'] = self.smali
        self.source_code['java'] = self.java

        return self.source_code

    def show_path(self, path):
        """show class name, method name and descriptor. Will call decompile_method"""

        class_manager = self.dalvik_executable.get_class_manager()

        try:
            src_class_name, src_method_name, src_descriptor =  path.get_src(class_manager)
            dst_class_name, dst_method_name, dst_descriptor =  path.get_dst(class_manager)

        except AttributeError:
            #permission like FACTORY_TEST caused the script to crash because : no src
            return None

        if not self.quiet:
            print "[-] %s->%s%s ---> %s->%s%s" % ( 
                                                  src_class_name,
                                                  src_method_name,
                                                  src_descriptor,
                                                  dst_class_name,
                                                  dst_method_name,
                                                  dst_descriptor)

        method = src_class_name + src_method_name + src_descriptor
        
        self.methods.append(method)

    def decompile_methods(self):
        """For each methods in self.methods, it will call decompile_method() on it."""

        if not self.quiet:
            print "[*] Will now try to decompile each method printed before"

        for method in self.methods:
            self.decompile_method(method)

    def decompile_method(self, method_to_decompile):
        """This function will decompile the given method. Maybe there is a better way. """
        if not self.quiet:
            print "Method to decompile :", method_to_decompile
            print "------------"
            
        for method in self.dalvik_executable.get_methods():
            mx = self.analyzed_dex.get_method(method)

            if method.get_code() == None:
                continue
            
            method_decompiling = method.get_class_name() + method.get_name() + method.get_descriptor()

            if method_to_decompile != method_decompiling:
                #debug
                #if not self.quiet:
                #    print method_decompiling
                continue

            if not self.quiet:
                print "About to decompile the following method : "
                print method.get_class_name(), method.get_name(), method.get_descriptor()
                print "[*] Decompilation initated"

            self.smali += [''.join((method.get_class_name(), method.get_name(), method.get_descriptor()))]
            
            ms = decompile.DvMethod(mx)
            
            if not self.quiet:
                print "[*] Processing..."
            try:
                ms.process()
            except:
                return None

            java_source = ms.get_source()

            if not self.quiet:
                print java_source
            
            self.java += [java_source]

class Main:

    def __init__(self, quiet = False):

        self.quiet = quiet
        self.first_time = True
        self.found_permissions = []

    def analyze_single_file(self, filename):

        if not self.quiet:
            print "[*] File %s given as input" % filename

        vulnerabilityFinder = VulnerabilityFinder(filename, self.quiet)
        vulnerabilityFinder.parse_input_file()
        permissions = vulnerabilityFinder.get_permissions()

        #create_new => override existing databse if it already exists
        if self.first_time == True:
            harvester = Harvester(create_new=True)
            self.first_time = False
        else:
            harvester = Harvester()

        harvester.add_app(filename, "Samsung", "200Kb", "system")
        app_id = harvester.get_last_app_id()

        #stores every permission into database
        if type(permissions) is None:
            print "[!] %s is an invalid file => no permissions"
            return None

        else:
            print type(permissions)
            if not permissions:
                print "coucou"
                return None

        if len(permissions)>0:
            
            for permission in permissions:
                
                if permission == "FACTORY_TEST":
                    print "[!] Skipping permissions FACTORY_TEST"
                    continue

                print "Current permission : %s" % (permission)
                
                if not permission in self.found_permissions:
                    harvester.add_permission(permission)
                    self.found_permissions += [permission]

                permission_id = harvester.get_last_permission_id()
                source_code = vulnerabilityFinder.get_permission_code(permission)

                if len(source_code['smali']) and len(source_code['java'])>0:
                    
                    if len(source_code['smali']) != len(source_code['java']):
                        print "An error probably occured : smali and java source code lists are not the same length"
                    i, j = 0,0
                    
                    while i < len(source_code['smali']) and j < len(source_code['java']):
                        smali_code = source_code['smali'][i]
                        java_code = source_code['java'][j]
                        harvester.add_usage(app_id, permission_id, smali_code, java_code)
                        i+=1
                        j+=1
                else:
                    print "An error occured : no source code for permission %s" % (permission)

    def get_targets_list(self, directory):
        if not self.quiet:
            print "[*] Directory %s given as input" % directory

        files = os.listdir(directory)


        print directory

        #directory_path = os.path.abspath(directory)[:os.path.abspath(directory).rfind('/')] + '/'

        if not self.quiet:
            print "[*] %d files found" % (len(files))

        for single_file in files:
            self.analyze_single_file(directory + single_file)
    

    def execute(self):
        """Here, we parse the commandline options and react according to them"""

        parser = argparse.ArgumentParser(description="A simple vulnerability finder")

        #either we analyze an entire directory or a single file, not both
        group = parser.add_mutually_exclusive_group()
        group.add_argument("-f", "--file", metavar="FILE", help="File to analyze", dest="filename")
        group.add_argument("-d", "--directory", metavar="DIRECTORY", help="Directory containing the files to analyze", dest="directory")
        parser.add_argument("-q", "--quiet", dest="quiet", action="store_true")

        options = parser.parse_args()

        #if no input files specified
        if options.directory==None and options.filename==None:
            parser.error("[!] You need to feed me with files to analyze")
            parser.print_help()
            sys.exit(1)

        #directory as input
        elif options.directory:
            self.get_targets_list(options.directory)

        #single file as input
        else:
            self.analyze_single_file(options.filename)

if __name__ == "__main__":
    main = Main()
    main.execute()
