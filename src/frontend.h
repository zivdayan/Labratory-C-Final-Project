struct ast {
    char error[200];
    enum {
        ast_inst,
        ast_dir,
        ast_define,
        ast_empty
    } ast_type;
    union {
        struct {
            const char *label;
            int number;
        } define;


        struct {
            enum {
                ast_extern,
                ast_entry,
                ast_string,
                ast_data
            } dir_type;

            union {
                char * label;
                char * string;
                struct {
                    enum {
                        data_label,
                        data_number
                    } data_type;
                    union {
                        char *label;
                        int number;
                    }
                } data[80];
            } dir_options;

        } dir;

    } ast_options;

};

struct ast get_ast_from_line(char* line);
