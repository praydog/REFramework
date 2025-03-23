from ctypes import addressof
import os
import fire
import pefile
import sys
import numpy as np
import pickle
import json

from unicorn import *
from unicorn.x86_const import *

from capstone import *
from capstone.x86_const import *

BASE = 0x140000000
EMU_START = 0x1461743F0
EMU_END = EMU_START + 0x10000

REGISTER_MEMORY_SIZE = 1024 * 1024

FILL_BYTE = 16

zero_member_functions = {}

# these are chains we'll use for testing on games we are encountering issues with
# so we don't need to parse the entire JSON dump
default_chains = {
    "via.Component": {
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x140001560",
                "name": "via.Component"
            }
        ],
    },
    "via.Behavior": {
        "address": "14d7af720",
        "crc": "d4512561",
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x140001560",
                "name": "via.Component"
            },
            {
                "address": "0x1400c0150",
                "name": "via.Behavior"
            }
        ],
    },
    "via.motion.Motion": {
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x140001560",
                "name": "via.Component"
            },
            {
                "address": "0x14a5ce960",
                "name": "via.motion.Animation"
            },
            {
                "address": "0x149d3c400",
                "name": "via.motion.Motion"
            }
        ]
    },
    "via.physics.RequestSetCollider": {
        "address": "14db56c20",
        "crc": "1be418e8",
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x140001560",
                "name": "via.Component"
            },
            {
                "address": "0x1401aa990",
                "name": "via.physics.CollidableBase"
            },
            {
                "address": "0x1401ef9c0",
                "name": "via.physics.RequestSetCollider"
            }
        ],
    },
    "via.landscape.layer.LandformBaseLayer": {
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x14bee7b10",
                "name": "via.landscape.layer.LandformBaseLayer"
            },
        ],
    },
    "via.landscape.layer.OutputLayer": {
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x14bee7b10",
                "name": "via.landscape.layer.LandformBaseLayer"
            },
            {
                "address": "0x14bee7b10",
                "name": "via.landscape.layer.OutputLayer"
            }
        ],
    },
    "via.behaviortree.Condition": {
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x149d1eb10",
                "name": "via.behaviortree.Condition"
            },
        ],
    },
    "via.motion.Fsm2ConditionMotionEnd": {
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x149d1eb10",
                "name": "via.behaviortree.Condition"
            },
            {
                "address": "0x14becd500",
                "name": "via.motion.Fsm2ConditionMotionEnd"
            }
        ],
    },
    "app.SoundSpatialAudioManager": {
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x140001560",
                "name": "via.Component"
            },
            {
                "address": "0x1400c0150",
                "name": "via.Behavior"
            }
        ],
    },
    "via.Transform": {
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x140001560",
                "name": "via.Component"
            },
            {
                "address": "0x149d16170",
                "name": "via.Transform"
            }
        ],
    },
    "via.render.Mesh": {
        "deserializer_chain": [
            {
                "address": "0x14a2f77d0",
                "name": "via.Object"
            },
            {
                "address": "0x149e909c0",
                "name": "System.Object"
            },
            {
                "address": "0x140001560",
                "name": "via.Component"
            },
            {
                "address": "0x149f1b8c0",
                "name": "via.render.Mesh"
            }
        ],
    }
}

class Allocator:
    def __init__(self, emu, start_alloc):
        self.emu = emu
        self.align = 4 * 1024
        self.alloc_addr = start_alloc
        self.alloc_addr = (self.alloc_addr + (self.align - 1)) & ~(self.align - 1)

    def allocate(self, size, prot=UC_PROT_ALL):
        size = (size + (self.align - 1)) & ~(self.align - 1)
        out = self.alloc_addr

        self.emu.mem_map(self.alloc_addr, size, prot)
        self.alloc_addr = (self.alloc_addr + size + 1 + (self.align - 1)) & ~(self.align - 1)

        print("Allocated memory at %X with size %X" % (out, size))

        return out

allocator = None

def invalidate_and_return_call(emu, frame, nop_out=True):
    cs = frame["cs"]

    # Load the context before the previous call
    ctx = pickle.loads(frame["call_stack"][1]["context"])
    emu.context_restore(ctx)

    rip = emu.reg_read(UC_X86_REG_RIP)
    print("Invalidating %X" % rip)

    frame["call_stack"] = frame["call_stack"][0:1]

    # Fix the history count for RIP
    if len(frame["call_stack"]) > 0 and rip in frame["call_stack"][-1]["history"]:
        dis_g = cs.disasm(emu.mem_read(rip, 0x100), rip, 1)
        dis = next(dis_g)

        hist = frame["call_stack"][-1]["history"]

        for i in range(rip, rip + dis.size):
            hist[i] = hist[i] - 1

    dis = next(cs.disasm(emu.mem_read(rip, 0x100), rip, 1)) # Disassembling because not all call variants are E8
    # emu.mem_write(emu.reg_read(UC_X86_REG_RIP), b"\x90\x90\x90\x90\x90")
    if nop_out == True:
        nops = b"\x90" * dis.size
        emu.mem_write(rip, nops)
    
    emu.reg_write(UC_X86_REG_RAX, 0)
    emu.reg_write(UC_X86_REG_RDX, 0)
    emu.reg_write(UC_X86_REG_RCX, 0)
    emu.reg_write(UC_X86_REG_R8, 0)
    emu.reg_write(UC_X86_REG_R9, 0)

    if nop_out == True:
        frame["start"] = emu.reg_read(UC_X86_REG_RIP)
    else:
        frame["start"] = emu.reg_read(UC_X86_REG_RIP) + dis.size
    
    # Try to read the new address to make sure it's valid memory
    try:
        emu.mem_read(frame["start"], 1)
    except:
        print("Failed to read new address 0x%X" % frame["start"])
        return

# callback for tracing invalid memory access (READ or WRITE)
def hook_mem_invalid(emu, access, address, size, value, frame):
    if access == UC_MEM_WRITE_UNMAPPED:
        print(">>> Missing memory is being WRITE at 0x%x, data size = %u, data value = 0x%x" \
                %(address, size, value))
        # map this memory in with 2MB in size
        frame["allocator"].allocate(2 * 1024*1024)
        # return True to indicate we want to continue emulation
        return True
    else:
        # return False to indicate we want to stop emulation
        return False

import traceback

def hook_code(emu, address, size, frame):
        # print("%x" % address)

        frame["context"] = pickle.dumps(emu.context_save())
        cs = frame["cs"]
        deserialize_arg = frame["deserialize_arg"]

        # We don't want to do this. We manually call each parent deserializer to mark where in the structure they start.
        # It's also easier to manage this way, we don't have to worry about nested shit.
        if len(frame["call_stack"]) > 1 and address in frame["deserializers"]:
            print("STOPPING EXECUTION!!!!")

            # Advance stream pointer as we've already emulated the parent and know where it ends
            if address in frame["deserializer_maxs"]:
                emu.mem_write(frame["deserialize_arg"] + 0x8, frame["deserializer_maxs"][address].to_bytes(8, sys.byteorder))
                print("[precomputed] Advanced stream pointer due to parent already being deserialized")

            invalidate_and_return_call(emu, frame, nop_out=False)
            emu.emu_stop()
            return

        # Upon first address in a call
        if len(frame["call_stack"]) > 0 and frame["call_stack"][-1]["first"] == True:
            frame["call_stack"][-1]["first"] = False

            # When the deserialize function calls another function,
            # We only care when it calls a function that reads the stream for deserialization
            # Any other function is irrelevant to us
            if emu.reg_read(UC_X86_REG_RCX) != deserialize_arg:
                invalidate_and_return_call(emu, frame)
                emu.emu_stop()
                return
                

        '''
        try:
            emu.mem_read(address, 4)
        except unicorn.UcError as e:
            #frame["call_stack"].pop()
            return False
        '''

        # print("%X %i" % (address, size))

        try:
            # Read 15 bytes as that's the max size of an instruction on x86_64
            # previously this was failing patheticly on a vbroadcastss instruction
            # which is quite a rare instruction and also quite large
            dis = next(cs.disasm(emu.mem_read(address, 15), address, 1))
        except Exception as e:
            print(traceback.format_exc(), "EXCEPTION 0x%X size 0x%x" % (address, size))
            print(" ", emu.mem_read(address, 0x100).hex())
            print("%X" % frame["call_stack"][-1]["last_executed_addr"])
            os.system("pause")
        # print("0x%x: %s %s" % (address, dis.mnemonic, dis.op_str))

        #if dis.prefix == "lock":
        if emu.mem_read(address, 1) == b"\xF0":
            print("LOCK PREFIX DETECTED! 0x%X %i" % (address, dis.size))
            # Skip over instruction
            #emu.reg_write(UC_X86_REG_RIP, address + dis.size)
            next_dis = next(cs.disasm(emu.mem_read(address + dis.size, 15), address + dis.size, 1))
            # Check if next insn is a conditional jmp and nop it out
            if next_dis.mnemonic == "jne":
                print("JNE detected after lock prefix, NOPing out")
                nops = b"\x90" * next_dis.size
                emu.mem_write(address + dis.size, nops)

            return

        lex = 0

        if len(frame["call_stack"]) > 0:
            lex = frame["call_stack"][-1]["last_executed_addr"]

        if len(frame["call_stack"]) == 1:
            cur_hist = frame["call_stack"][0]["history"]

            # We just left a loop
            if address not in cur_hist.keys() and lex in cur_hist.keys() and cur_hist[lex] > 1:
                list_size = cur_hist[lex] + 1

                # Corrections for for loops vs do while loops
                if list_size == FILL_BYTE + 1:
                    list_size = FILL_BYTE
                
                # print("Multiple execution @ 0x%X (%i)" % (lex, list_size))

                # Loop count matches the integer we filled the whole buffer with
                if list_size == FILL_BYTE and len(frame["layout"]) > FILL_BYTE and int.from_bytes(emu.mem_read(frame["deserialize_arg"] + 0x8, 8), sys.byteorder) > frame["max_deserialize_cur"]:
                    try:
                        element_layout = frame["layout"][-1]
                    except IndexError as e:
                        cs.detail = True
                        dis_g = cs.disasm(emu.mem_read(lex, 0x100), address, 1)
                        dis = next(dis_g)

                        print("LEX: 0x%x" % lex)
                        print("0x%x: %s %s" % (address, dis.mnemonic, dis.op_str))

                        print("Instruction at %X didn't read bytes from stream?" % address)
                        os.system("pause")
                        return

                    # Erase the elements that were added to the layout, keep list only
                    frame["layout"] = frame["layout"][0:len(frame["layout"]) - FILL_BYTE]
                    frame["was_string"] = False

                    try:
                        list_layout = frame["layout"][-1]
                    except IndexError as e:
                        cs.detail = True
                        dis_g = cs.disasm(emu.mem_read(lex, 0x100), address, 1)
                        dis = next(dis_g)

                        print("LEX: 0x%x" % lex)
                        print("0x%x: %s %s" % (address, dis.mnemonic, dis.op_str))

                        print("Instruction at %X didn't read bytes from stream? (2)" % address)
                        os.system("pause")
                        return
                    
                    list_layout["list"] = True
                    list_layout["element"] = element_layout
                    list_layout["element_size"] = int((element_layout["offset"] - list_layout["offset"]) / FILL_BYTE)

                    # print("LIST DETECTED")

        # easy way to wait until after insn executes to read stuff
        if lex > 0:
            cs.detail = True

            try:
                last_dis_g = cs.disasm(emu.mem_read(lex, 0x100), address, 1)
            except Exception as e:
                print(traceback.format_exc(), "LEX EXCEPTION 0x%X 0x%X" % (address, lex))

                for i in range(0, len(frame["call_stack"])):
                    print("0x%X" % frame["call_stack"][i]["last_executed_addr"])

                print(" ", emu.mem_read(address, 0x100).hex())
                print("%X" % frame["call_stack"][-1]["last_executed_addr"])
                os.system("pause")
            
            last_dis = next(last_dis_g)

            deserialize_cur = int.from_bytes(emu.mem_read(frame["deserialize_arg"] + 0x8, 8), sys.byteorder)
            has_operands = len(last_dis.operands) > 0

            if last_dis.mnemonic == "mov" and has_operands and last_dis.operands[0].type == X86_OP_REG:
                val = emu.reg_read(last_dis.operands[0].reg)

                if val == deserialize_cur:
                    # print("0x%X" % val)
                    frame["last_deserialize_reg"] = last_dis.operands[0].reg
                    frame["last_deserialize_reg_val"] = val
                    # print("0x%x: %s %s" % (address, last_dis.mnemonic, last_dis.op_str))

                # print("0x%x: %s %s %s" % (address, dis.mnemonic, dis.op_str, dis.reg_name(dis.operands[0].reg)))
            elif frame["last_deserialize_reg"] != -1:
                if deserialize_cur != frame["last_deserialize_cur"]:
                    frame["last_deserialize_reg"] = -1
                    frame["last_deserialize_reg_val"] = 0

                    # print("0x%x: %s %s" % (address, last_dis.mnemonic, last_dis.op_str))
                elif has_operands and last_dis.operands[0].type == X86_OP_REG:
                    val = emu.reg_read(last_dis.operands[0].reg)

                    if val != frame["last_deserialize_reg_val"]:
                        delta = val - frame["last_deserialize_reg_val"]

                        if abs(delta) > 0x10000:
                            print("Huge delta detected. Register overwritten? 0x%X" % lex)
                            # frame["last_deserialize_reg"] = -1
                            # frame["last_deserialize_reg_val"] = 0
                            # invalidate_and_return_call(emu, frame)
                            # os.system("pause")

                    if last_dis.mnemonic == "and" and last_dis.operands[1].type == X86_OP_IMM and last_dis.operands[0].reg == frame["last_deserialize_reg"]:
                        # print("0x%X alignment detected" % (~last_dis.operands[1].imm + 1))

                        # Set this because we don't want the actual byte count screwing up
                        frame["last_deserialize_cur"] = val
                        frame["last_alignment"] = (~last_dis.operands[1].imm + 1)

                    frame["last_deserialize_reg_val"] = val
            elif frame["last_alignment"] == 4 and last_dis.group(X86_GRP_BRANCH_RELATIVE):
                frame["was_string"] = True
            elif frame["last_alignment"] == 4 and last_dis.bytes == bytearray(b"\x4B\x8D\x0C\x41"): # this means "lea rcx, [r9+r8*2]", e.g. reading a wide string
                frame["was_string"] = True
                #print("String or list detected")
            elif frame["last_alignment"] == 4 and last_dis.bytes == bytearray(b"\x4A\x8D\x14\x4A"): # this means "lea rdx, [rdx+r9*2]", e.g. reading a wide string
                frame["was_string"] = True
                #print("String or list detected")

            cs.detail = False

        # Keep track of how many times we've executed the instruction at this address
        if dis.mnemonic != "ret":
            counter = 0

            # SNEAKY!
            # unicorn runs this instruction multiple times through the callback to emulate it
            if dis.mnemonic == "rep movsb":
                counter = emu.reg_read(UC_X86_REG_ECX)

            if len(frame["call_stack"]) > 0:
                # Keep track of all the bytes of the instruction because we NOP them out sometimes
                for i in range(address, address + dis.size):
                    history = frame["call_stack"][-1]["history"]

                    if i not in history:
                        history[i] = 0

                    if counter == 0:
                        history[i] = history[i] + 1

                        if len(frame["call_stack"]) == 1 and counter > FILL_BYTE:
                            print("YUP", history[i])

                        # if dis.mnemonic == "rep movsb":
                            # print("YUP", history[i])

        if dis.mnemonic == "call":
            is_normal_call = dis.bytes[0] == 0xE8
            frame["call_stack"].append({ 
                "addr": address + dis.size,
                "context":  pickle.dumps(emu.context_save()),
                "history": {},
                "last_executed_addr": 0,
                "first": is_normal_call
            })

        # Potential return from read func
        if dis.mnemonic == "ret":
            frame["last_return_val"] = emu.reg_read(UC_X86_REG_RAX)

            if len(frame["call_stack"]) > 0:
                frame["call_stack"].pop()

                deserialize_cur = int.from_bytes(emu.mem_read(deserialize_arg + 0x8, 8), sys.byteorder)

                if deserialize_cur > frame["last_deserialize_cur"]:
                    delta = deserialize_cur - frame["last_deserialize_cur"]

                    # print("0x%X bytes, 0x%X alignment" % (delta, frame["last_alignment"]))

                    if deserialize_cur > frame["max_deserialize_cur"]: # This stop duplicates from inlined descendants from leaking into this.
                        frame["layout"].append({ 
                            "size": delta,
                            "element_size": delta,
                            "element": None,
                            "align": frame["last_alignment"],
                            "string": frame["was_string"],
                            "list": False,
                            "offset": deserialize_cur - frame["buffer_start"]
                        })

                        frame["last_layout_size"] = len(frame["layout"])
                    frame["was_string"] = False

                frame["last_deserialize_reg"] = -1
                frame["last_deserialize_reg_val"] = 0
                frame["last_deserialize_cur"] = deserialize_cur
                frame["last_alignment"] = 1

                if len(frame["call_stack"]) == 0:
                    # print("Reached end of function call")
                    frame["start"] = EMU_END
                    emu.emu_stop()
            else:
                print("Reached end of function call in a BAD WAY")
                frame["start"] = EMU_END
                emu.emu_stop()
        else:
            # Allows us to detect stream modifications that are inline
            if len(frame["call_stack"]) == 1: # HACK: this is a fix for string read functions that are not inlined. Not future proof!
                deserialize_cur = int.from_bytes(emu.mem_read(deserialize_arg + 0x8, 8), sys.byteorder)
                
                if cur_hist[address] > FILL_BYTE:
                    #print("WARNING: Multiple execution of instruction at 0x%X (%i)" % (address, cur_hist[address]))

                    if cur_hist[address] > 200:
                        print("WARNING: Too many executions of instruction at 0x%X, STOPPING EXECUTION!" % address)
                        frame["start"] = EMU_END
                        emu.emu_stop()
                        return

                if deserialize_cur > frame["last_deserialize_cur"]: # != is naive, it must be greater to get past alignment
                    # check if we've already executed this instruction
                    cur_hist = frame["call_stack"][-1]["history"]

                    delta = deserialize_cur - frame["last_deserialize_cur"]

                    if deserialize_cur > frame["max_deserialize_cur"]: # This stop duplicates from inlined descendants from leaking into this.
                        frame["layout"].append({ 
                            "size": delta,
                            "element_size": delta,
                            "element": None,
                            "align": frame["last_alignment"],
                            "string": frame["was_string"],
                            "list": False,
                            "offset": deserialize_cur - frame["buffer_start"]
                        })

                        frame["last_layout_size"] = len(frame["layout"])
                    frame["was_string"] = False
                    
                    frame["last_deserialize_reg"] = -1
                    frame["last_deserialize_reg_val"] = 0
                    frame["last_alignment"] = 1
                
                    frame["last_deserialize_cur"] = deserialize_cur

        if len(frame["call_stack"]) > 0:
            frame["call_stack"][-1]["last_executed_addr"] = address

        frame["last_disasm"] = dis

# Hook for valid memory reads.
def hook_read(emu, access, address, size, value, frame):
    buffer_start = frame["buffer_start"]
    buffer_end = frame["buffer_end"]

    if address >= buffer_start and address < buffer_end:
        if size <= 32:
            #replacement_value = int(FILL_BYTE).to_bytes(size, sys.byteorder)  # 8 bytes representing the integer 16
            emu.mem_write(address, np.zeros(size, dtype=np.byte).tobytes())
            emu.mem_write(address, int(FILL_BYTE).to_bytes(1, sys.byteorder))

            #print("%i bytes read at 0x%X" % (size, emu.reg_read(UC_X86_REG_RIP)))

# Nops out instructions that are causing problems so we can continue execution
def hook_unmapped(emu, access, address, size, value, frame):
    # Execution has left the scope of the function.
    if len(frame["call_stack"]) == 0:
        frame["start"] = EMU_END
        emu.emu_stop()
        return False

    cs = frame["cs"]
    
    if len(frame["call_stack"]) > 1:
        invalidate_and_return_call(emu, frame)
    else:
        # Load the context before the previous instruction
        ctx = pickle.loads(frame["context"])
        emu.context_restore(ctx)

        rip = emu.reg_read(UC_X86_REG_RIP)
        
        dis_g = cs.disasm(emu.mem_read(rip, 0x100), rip, 1)
        dis = next(dis_g)

        # only NOP out non-return instructions
        # otherwise weird things happen
        if dis.mnemonic != "ret":
            nops = np.empty(dis.size, dtype=np.byte)
            nops.fill(0x90)

            # Fix the history count for RIP
            if len(frame["call_stack"]) > 0 and rip in frame["call_stack"][-1]["history"]:
                hist = frame["call_stack"][-1]["history"]

                for i in range(rip, rip + dis.size):
                    hist[i] = hist[i] - 1

            # Nop out the instruction causing read/write errors
            emu.mem_write(rip, nops.tobytes())
            #emu.reg_write(UC_X86_REG_RIP, rip + dis.size)
        else:
            print("WTF? WE TRIED TO NOP OUT A RET? 0x%X" % rip)

    frame["start"] = emu.reg_read(UC_X86_REG_RIP)

    # Continue execution
    return True

def hook_write(emu, access, address, size, value, frame):
    if address >= frame["fake_vtable"] and address < frame["fake_vtable"] + REGISTER_MEMORY_SIZE:
        print("Something bad is happening with the vtable!!!!! WTF 0x%X" % emu.reg_read(UC_X86_REG_RIP))
        os.system("pause")
        return False

    if address == frame["fake_return_func"]:
        print("ATTEMPTED WRITE TO RETURN FUNC 0x%X!" % emu.reg_read(UC_X86_REG_RIP))
        os.system("pause")
        return False

    return True

def hook_invalid_insn(emu, frame):
    rip = emu.reg_read(UC_X86_REG_RIP)

    try:
        print("Invalid instruction at 0x%X, attempting to ignore..." % rip)
        cs = frame["cs"]

        dis = next(cs.disasm(emu.mem_read(rip, 0x100), rip, 1))
        print("0x%x: %s %s" % (rip, dis.mnemonic, dis.op_str))

        # Advance to the next instruction
        frame["start"] = emu.reg_read(UC_X86_REG_RIP)
        emu.reg_write(UC_X86_REG_RIP, rip + dis.size)
        nops = b"\x90" * dis.size
        emu.mem_write(rip, nops)
    except Exception as e:
        print("Failed to disassemble 0x%X" % rip)
        return


def verify_file(p):
    if not os.path.exists(p):
        print("Path %s does not exist" % p)
        return False

    if not os.path.isfile(p):
        print("Path %s is not a file!" % p)
        return False

    return True

def main(p, il2cpp_path="il2cpp_dump.json", test_mode=False):
    if not verify_file(p) or not verify_file(il2cpp_path):
        return

    # test_mode = True

    pe_filename = os.path.basename(p)

    if test_mode == False:
        with open(il2cpp_path, "r", encoding="utf8") as f:
            chains = json.load(f)
    else:
        chains = default_chains

    chains_len = len(chains)

    data = b''

    with open(p, "rb") as f:
        data = f.read()

    struct_file = open("dump_ " + pe_filename + ".txt", "w")

    print("Opened with length 0x%X" % len(data))

    emu = Uc(UC_ARCH_X86, UC_MODE_64)
    cs = Cs(CS_ARCH_X86, CS_MODE_64)
    pe = pefile.PE(data=data, fast_load=True)
    
    map_align = 4 * 1024

    highest_alloc = 0

    # Map the sections into unicorn
    for section in pe.sections:
        print(section.Name.decode("utf-8"), hex(section.VirtualAddress), hex(section.PointerToRawData))
        start = section.PointerToRawData
        end = start + section.SizeOfRawData

        # print(section)

        va = BASE + section.VirtualAddress

        aligned_virtual_size = section.Misc_VirtualSize
        aligned_virtual_size = (aligned_virtual_size + (map_align - 1)) & ~(map_align - 1)

        print(" 0x%X -> 0x%X" %  (va + section.Misc_VirtualSize, va + aligned_virtual_size))
        
        emu.mem_map(va, aligned_virtual_size, UC_PROT_ALL)
        emu.mem_write(va, data[start:end])

        if (va + aligned_virtual_size) > highest_alloc:
            highest_alloc = va + aligned_virtual_size

    highest_alloc = (highest_alloc + 1024 + (map_align - 1)) & ~(map_align - 1)
    allocator = Allocator(emu, highest_alloc + (1024 + (map_align - 1)) & ~(map_align - 1))
    
    '''
    pe.parse_data_directories()
    for entry in pe.DIRECTORY_ENTRY_IMPORT:
        print(entry.dll)
        for imp in entry.imports:
            print('\t', hex(imp.address), imp.name)
    '''

    arg_names = {
        "rcx": UC_X86_REG_RCX, 
        "rdx": UC_X86_REG_RDX, 
        "r8": UC_X86_REG_R8, 
        "r9": UC_X86_REG_R9,
        "rsp": UC_X86_REG_RSP
    }

    args = {}

    register_zeroes = np.zeros(REGISTER_MEMORY_SIZE, dtype=np.byte).tobytes()

    def init_registers():
        # write the register values
        for name, value in arg_names.items():
            if name not in args:
                args[name] = allocator.allocate(REGISTER_MEMORY_SIZE)
            else:
                emu.mem_write(args[name], register_zeroes)
            
            emu.reg_write(value, args[name])

    init_registers()

    # Initialize the deserializer buffer
    deserialize_buffer = allocator.allocate(REGISTER_MEMORY_SIZE)
    deserialize_arg = args["rdx"]

    # Initialize fake return func for vtable
    fake_return_func = allocator.allocate(1024,)

    # xor rax, rax
    # ret
    emu.mem_write(fake_return_func, b'\x48\x31\xc0\xc3')

    # Initialize fake vtable for object in RCX
    fake_vtable = allocator.allocate(REGISTER_MEMORY_SIZE)

    print("Fake vtable allocated at 0x%X" % fake_vtable)

    print("Fake return func allocated at 0x%X" % fake_return_func)

    for i in range(0, int(REGISTER_MEMORY_SIZE / 8)):
        emu.mem_write(fake_vtable + (i * 8), fake_return_func.to_bytes(8, sys.byteorder))

    # def init_vtable():

    meta_frame = {
        "cs": cs,
        "deserialize_arg": deserialize_arg,
        "last_deserialize_cur": int.from_bytes(emu.mem_read(deserialize_arg + 0x8, 8), sys.byteorder),
        "max_deserialize_cur": 0,
        "current_deserializer": 0,
        "deserializer_layouts": {},
        "deserializer_maxs": {},
        "allocator": allocator,
        "last_deserialize_reg": -1,
        "last_deserialize_reg_val": 0,
        "last_disasm": None,
        "last_alignment": 1,
        "call_stack": [],
        "layout": [],
        "was_string": False,
        "last_return_val": 0,
        "last_layout_size": 0,
        "fake_vtable": fake_vtable,
        "fake_return_func": fake_return_func,
        "buffer_start": deserialize_buffer,
        "buffer_end": deserialize_buffer + REGISTER_MEMORY_SIZE
    }

    emu.hook_add(UC_HOOK_CODE, hook_code, meta_frame)
    # emu.hook_add(UC_HOOK_MEM_INVALID, hook_mem_invalid, meta_frame)
    emu.hook_add(UC_HOOK_INSN_INVALID, hook_invalid_insn, meta_frame)
    emu.hook_add(UC_HOOK_MEM_FETCH_UNMAPPED, hook_unmapped, meta_frame)
    emu.hook_add(UC_HOOK_MEM_READ_UNMAPPED, hook_unmapped, meta_frame)
    emu.hook_add(UC_HOOK_MEM_WRITE_UNMAPPED, hook_unmapped, meta_frame)
    emu.hook_add(UC_HOOK_MEM_WRITE, hook_write, meta_frame)
    emu.hook_add(UC_HOOK_MEM_READ, hook_read, meta_frame)

    pristine_context = pickle.dumps(emu.context_save())

    def detect_members(deserializer_start, prev_entries):
        if deserializer_start in zero_member_functions:
            return []
        
        if deserializer_start in prev_entries:
            print("Detected duplicate deserializer call at 0x%X" % deserializer_start)
            return []
        
        if deserializer_start in meta_frame["deserializer_layouts"]:
            if deserializer_start in meta_frame["deserializer_maxs"]:
                meta_frame["max_deserialize_cur"] = meta_frame["deserializer_maxs"][deserializer_start]
            
            return meta_frame["deserializer_layouts"][deserializer_start]

        # print("Detecting members for deserializer %X" % deserializer_start)

        '''
        struct DeserializeStream
        {
            uint8_t* head;
            uint8_t* cur;
            uint8_t* tail;
            uint64_t stackptr;
            FixedArray<unsigned char *,32> stack;
        };
        '''

        init_registers()
        # init_vtable()

        # Initialize the deserializer struct
        emu.mem_write(deserialize_arg + 0x0, deserialize_buffer.to_bytes(8, sys.byteorder)) # head
        emu.mem_write(deserialize_arg + 0x8, deserialize_buffer.to_bytes(8, sys.byteorder)) # cur
        emu.mem_write(deserialize_arg + 0x10, (deserialize_buffer + REGISTER_MEMORY_SIZE).to_bytes(8, sys.byteorder)) # tail
        emu.mem_write(deserialize_arg + 0x18, (deserialize_arg + 0x20).to_bytes(8, sys.byteorder))

        # Initialize the buffer with some ones
        deserialize_data = np.ones(int(REGISTER_MEMORY_SIZE / 4), dtype=np.uintc) * FILL_BYTE
        emu.mem_write(deserialize_buffer, deserialize_data.tobytes())

        # Set up the stack
        initial_rsp = args["rsp"]
        emu.reg_write(arg_names["rsp"], initial_rsp + int(REGISTER_MEMORY_SIZE / 2))
        emu.mem_write(args["rsp"], register_zeroes)

        # Write the fake vtable to the object
        emu.mem_write(args["rcx"], fake_vtable.to_bytes(8, sys.byteorder))

        meta_frame["call_stack"] = []

        # Our "call stack" which just records data for the current call scope
        meta_frame["call_stack"].append({ 
            "addr": deserializer_start, 
            "context":  pickle.dumps(emu.context_save()),
            "history": {},
            "last_executed_addr": 0,
            "first": False
        })

        meta_frame["current_deserializer"] = deserializer_start
        meta_frame["start"] = deserializer_start
        meta_frame["last_deserialize_reg"] = -1
        meta_frame["last_deserialize_reg_val"] = 0
        meta_frame["last_alignment"] = 1
        meta_frame["was_string"] = False
        meta_frame["last_deserialize_cur"] = int.from_bytes(emu.mem_read(deserialize_arg + 0x8, 8), sys.byteorder)
        start_deserialize_cur = meta_frame["last_deserialize_cur"]

        prev_layout_size = len(meta_frame["layout"])

        out_layout = []

        original_start = deserializer_start

        for i in range(0, 1000):
            # Execution has left the scope of the function.
            if len(meta_frame["call_stack"]) == 0:
                break

            if meta_frame["start"] == EMU_END:
                break

            try:
                emu.emu_start(meta_frame["start"], deserializer_start + 0x10000)
            except unicorn.UcError as e:
                print("RIP: 0x%X" % emu.reg_read(UC_X86_REG_RIP))
                print(e)
                #meta_frame["start"] = emu.reg_read(UC_X86_REG_RIP)
                continue

        layout_delta = len(meta_frame["layout"]) - prev_layout_size
        # print("Function contained %i members" % layout_delta)
        if layout_delta == 0:
            zero_member_functions[deserializer_start] = True
        else:
            out_layout = meta_frame["layout"][prev_layout_size:(prev_layout_size+layout_delta)]

        cur_deserialize_cur = int.from_bytes(emu.mem_read(deserialize_arg + 0x8, 8), sys.byteorder)

        if cur_deserialize_cur > start_deserialize_cur and cur_deserialize_cur > meta_frame["max_deserialize_cur"]:
            meta_frame["max_deserialize_cur"] = cur_deserialize_cur
        
        if cur_deserialize_cur > start_deserialize_cur:
            meta_frame["deserializer_maxs"][deserializer_start] = cur_deserialize_cur
        
        meta_frame["deserializer_layouts"][deserializer_start] = out_layout
        
        return out_layout

    # Detects members for one structure deserializer chain
    def detect_members_chain(struct_name, chain):
        meta_frame["layout"] = []
        meta_frame["max_deserialize_cur"] = 0
        # our dict to check if the deserializer calls a parent deserializer (and ignore it)
        meta_frame["deserializers"] = {int(address, 16): True for item in chain for (key, address) in item.items() if key == "address"}

        emu.context_restore(pickle.loads(pristine_context))
 
        layout_list = []
        prev_entries = {}

        # Actually detect the members now
        for entry in chain:
            addr = int(entry["address"], 16)
            #print("Detecting members for %s at 0x%X" % (entry["name"], addr))
            layout_list.append({
                "name": entry["name"],
                "layout": detect_members(addr, prev_entries)
            })

            prev_entries[addr] = True

        def generate_typename(layout):
            typename = ""

            if layout["list"] == True and layout["string"] == True:
                typename = "LIST AND STRING????? REPORT BUG"
            if layout["string"] == True:
                layout["size"] = layout["align"]
                typename = "String%iA%i" % (layout["size"], layout["align"])
            elif layout["list"] == True:
                typename = "List%iA%i" % (layout["size"], layout["align"])
                typename = typename + generate_typename(layout["element"])
            else:
                typename = "Data%iA%i" % (layout["size"], layout["align"])

            return typename

        # Construct the structure
        if len(meta_frame["layout"]) > 0:
            i = 0
            struct_str = "struct " + struct_name + " {\n"

            for parent in layout_list:
                if len(parent["layout"]) == 0:
                    continue

                struct_str = struct_str + "// " + parent["name"] + " BEGIN\n"

                for layout in parent["layout"]:
                    type_name = generate_typename(layout)
                    var_name = "v" + str(i)

                    struct_str = struct_str + "    "
                    struct_str = struct_str + type_name + " " + var_name + ";\n"

                    i = i + 1
                
                struct_str = struct_str + "// " + parent["name"] + " END\n"

            struct_str = struct_str + "};\n"
            print(struct_str)
            struct_file.write(struct_str)

            print("Native struct potentially has %i members" % len(meta_frame["layout"]))

        return layout_list

    count = 0
    native_layouts = {}

    print("Emulating...")

    for struct_name in chains.keys():
        if chains[struct_name] is not None and "deserializer_chain" in chains[struct_name]:
            print(struct_name)
            struct_layout = detect_members_chain(struct_name, chains[struct_name]["deserializer_chain"])

            if len(struct_layout) > 0:
                native_layouts[struct_name] = struct_layout

                if struct_name == "via.render.MaterialParam" or struct_name == "via.render.LocalCubemap":
                    des_addr = int(chains[struct_name]["deserializer_chain"][-1]["address"], 16)
                    des_disasm = cs.disasm(emu.mem_read(des_addr, 0x1000), des_addr, 1000)
                    # Print disassembly
                    print("Disassembly:")
                    for ins in des_disasm:
                        if ins.mnemonic == "ret":
                            break

                        print(ins)
                    os.system("pause")

        count = count + 1
        sys.stdout.write("\r%f%%" % (float(count / chains_len) * 100.0))

    print("Finished. Dumping to native_layouts.json")
    with open("native_layouts_" + pe_filename + ".json", "w") as f:
        json.dump(native_layouts, f, indent=4, sort_keys=True)

    

if __name__ == '__main__':
    fire.Fire(main)