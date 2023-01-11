module mem_manager_test();

    byte unsigned data[64];

    initial begin

        mem_manager::mm_t mm = mem_manager::create();

        for (int i = 0; i < 64; i++) data[i] = 8'(i);
        mem_manager::write_64(mm, 'h8000, data);

        for (int i = 0; i < 64; i++) data[i] = 8'd0;
        void'(mem_manager::read_64_sized(mm, 'h8000, 64, data));

        for (int i = 0; i < 64; i++) assert(data[i] == 8'(i)) else $error("bad data expected %0d actual %0d", i, data[i]);
        mem_manager::destroy(mm);

        $finish;

    end

endmodule
