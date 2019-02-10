# Install winappdbg from http://winappdbg.sourceforge.net/
from winappdbg import Debug, EventHandler, Process, System, CrashDump, HexInput, HexDump, win32

def process_read( pid, address, length ):
 
    process = Process( pid )
    # Read the process memory.
    data = process.read( address, length )
    return data
 
def action_WriteProcessMemoryW(event):
    # Get the return address of the call
    address = event.get_thread().read_stack_dwords(1)[0]
    fo = open("C:/unpacked.bin", "w")
 
    # Get the process and thread IDs
    pid     = event.get_pid()
    tid     = event.get_tid()
 
    process         = Process(pid)
 
    bufferAddr=event.get_thread().read_stack_dwords(6)[3] #6 is no of dwords grabbed from stack [0] is retuen addr [3] is 3rd argument
    print hex(bufferAddr)
    print "xxxxxxx"
    memoryMap       = process.get_memory_map()
    readable    = 0
    writeable   = 0
    executable  = 0
    private     = 0
    mapped      = 0
    image       = 0
    total       = 0
    for mbi in memoryMap:
        #print hex(mbi.BaseAddress)
        if mbi.BaseAddress == bufferAddr:
            print "dumping data"
            print hex(mbi.BaseAddress)
            print hex(mbi.RegionSize)
            data=process_read(pid,mbi.BaseAddress,mbi.RegionSize)
            fo.write(data)
            fo.close()
 
 
# This function will be called when our breakpoint is hit
def action_CreateProcessW( event ):
 
 
    address = event.get_thread().read_stack_dwords(1)[0]
    # Get the process and thread IDs
    pid     = event.get_pid()
    tid     = event.get_tid()
 
 
class MyEventHandler( EventHandler ):
 
    def load_dll( self, event ):
 
        # Get the new module object
        module = event.get_module()
 
        # If it's kernel32.dll...
        if module.match_name("kernel32.dll"):
 
            # Get the process ID
            pid = event.get_pid()
 
            # Get the address of CreateFile
            address = module.resolve( "CreateProcessW" )
            addressWPW = module.resolve( "WriteProcessMemory" )
 
            # Set a breakpoint at CreateFile
            event.debug.break_at( pid, address, action_CreateProcessW )
            event.debug.stalk_at( pid, addressWPW, action_WriteProcessMemoryW )
 
 
 
 
def simple_debugger( argv ):
 
    # Instance a Debug object, passing it the MyEventHandler instance
    debug = Debug( MyEventHandler() )
    try:
 
        debug.execv( argv )
 
        debug.loop()
 
    # Stop the debugger
    finally:
        debug.stop()
 
 
# When invoked from the command line,
# the first argument is an executable file theat needs tp be unpacked. Please provide absolute path
# and the remaining arguments are passed to the newly created process
if __name__ == "__main__":
    import sys
    simple_debugger(sys.argv[1:])
