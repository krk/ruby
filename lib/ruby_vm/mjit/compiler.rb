require 'mjit/insn_compiler'
require 'mjit/instruction'
require 'mjit/x86_assembler'

module RubyVM::MJIT
  # Compilation status
  KeepCompiling = :keep_compiling
  CantCompile = :cant_compile
  EndBlock = :end_block

  class Compiler
    # Ruby constants
    Qundef = Fiddle::Qundef

    attr_accessor :write_pos

    # @param mem_block [Integer] JIT buffer address
    def initialize(mem_block)
      @mem_block = mem_block
      @write_pos = 0
      @insn_compiler = InsnCompiler.new
    end

    # @param iseq [RubyVM::MJIT::CPointer::Struct]
    def compile(iseq)
      return if iseq.body.location.label == '<main>'
      iseq.body.jit_func = compile_iseq(iseq)
    rescue Exception => e
      # TODO: check --mjit-verbose
      $stderr.puts e.full_message
    end

    def write_addr
      @mem_block + @write_pos
    end

    private

    # ec -> RDI, cfp -> RSI
    def compile_iseq(iseq)
      addr = write_addr
      asm = X86Assembler.new

      index = 0
      while index < iseq.body.iseq_size
        insn = decode_insn(iseq.body.iseq_encoded[index])
        status = compile_insn(asm, insn)
        if status == EndBlock
          break
        end
        index += insn.len
      end

      asm.compile(self)
      addr
    end

    def compile_insn(asm, insn)
      case insn.name
      when :putnil then @insn_compiler.compile_putnil(asm)
      when :leave  then @insn_compiler.compile_leave(asm)
      else raise NotImplementedError, "insn '#{insn.name}' is not supported yet"
      end
    end

    def decode_insn(encoded)
      INSNS.fetch(C.rb_vm_insn_decode(encoded))
    end
  end
end
