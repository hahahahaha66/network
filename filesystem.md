C++17引入了<filesystem>头文件，提供了跨平台的文件操作功能引入了许多功能

#### 存储路径

path p1 = "home/user/file.txt";//绝对路径
path p2 = "subdir/file.txt";//相对路径

可以对路径进行以下操作
+ 父目录 ：p1.parent_path()
+ 文件名 ：p1.filename()
+ 扩展名 ：p1.extension()
+ 是否是绝对路径 ：p1.is_absolute()

#### 目录遍历

非递归遍历

    for(const auto& entry : directory_iterator(".")) {
        cout<<entry.path()<<endl;
    }


递归遍历

    for(const auto& entry : recursive_directory_iterator(".")) {
        cout<<entry.path()<<endl;
    }