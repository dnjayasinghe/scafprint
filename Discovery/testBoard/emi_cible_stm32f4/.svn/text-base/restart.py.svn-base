#!gdb
import sys
import gdb
import os

number_restarts = 1000

def on_stop(sig):
    global number_restarts
    if isinstance(sig, gdb.SignalEvent) and sig.stop_signal == "SIGSEGV":
        crash_file = "crash.file." + str( gdb.selected_inferior().pid)
        gdb.execute("set logging file " + crash_file)
        gdb.execute("set logging on")
        gdb.execute("where")
        gdb.execute("set logging off")
    if (number_restarts > 0):
            number_restarts -= 1
            gdb.execute("set confirm off")
            gdb.execute("kill")
            gdb.execute("run")


gdb.events.stop.connect (on_stop)
gdb.execute("set pagination off")
gdb.execute("run")
