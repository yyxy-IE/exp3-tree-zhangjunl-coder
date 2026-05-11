/**
 * 实验：目录树查看器（仿 Linux tree 命令）
 * 学号：2504020454  姓名：张俊良
 * 说明：请补全所有标记为 TODO 的函数体，不要修改其他代码。
 * 目录树查看器（仿 Linux tree 命令）
 * 完整实现版本（C语言，左孩子右兄弟二叉树）
 * 编译：gcc -o tree tree.c -std=c99
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// ================== 二叉树结点定义 ==================
typedef struct FileNode {
    char *name;                  // 文件/目录名
    int isDir;                   // 1:目录 0:文件
    struct FileNode *firstChild; // 左孩子：第一个子项
    struct FileNode *nextSibling;// 右兄弟：下一个同层项
} FileNode;

// ================== 函数声明 ==================
FileNode* createNode(const char *name, int isDir);
int cmpNode(const void *a, const void *b);
FileNode* buildTree(const char *path);
void printTree(FileNode *node, const char *prefix, int isLast);
int countNodes(FileNode *root);
int countLeaves(FileNode *root);
int treeHeight(FileNode *root);
void countDirFile(FileNode *root, int *dirs, int *files);
void freeTree(FileNode *root);
char* getBaseName(void);

// ================== 需要补全的函数 ==================

// 创建新结点（分配内存、复制字符串、初始化指针）
FileNode* createNode(const char *name, int isDir) {
    FileNode* node = (FileNode *)malloc(sizeof(FileNode));
    if(!node){
        perror("malloc");
    // TODO: 实现
    return NULL;
    }
    node->name = strdup(name); 
    if (!node->name) {
        perror("strdup");
        free(node);
        return NULL;
    }
    node->isDir = isDir;
    node->firstChild = NULL;
    node->nextSibling = NULL;
    return node;
}

// 比较函数，用于 qsort 对子项按名称排序
int cmpNode(const void *a, const void *b) {
    FileNode *nodeA = *(FileNode **)a;
    FileNode *nodeB = *(FileNode **)b;
    return strcmp(nodeA->name, nodeB->name);
}
    // TODO: 实现
    


// 递归构建目录树（核心难点）
FileNode* buildTree(const char *path) {
      DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return NULL;
    }

    // 提取目录名作为当前结点名
    const char *baseName = strrchr(path, '/');
    baseName = baseName ? baseName + 1 : path;

    FileNode *node = createNode(baseName, 1);
    if (!node) {
        closedir(dir);
        return NULL;
    }

    // 子结点存储到临时数组
    FileNode *children[1024];
    int childCount = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // 跳过 . 和 ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 拼接完整路径
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(fullPath, &st) != 0) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            FileNode *child = buildTree(fullPath);
            if (child) {
                children[childCount++] = child;
            }
        } else if (S_ISREG(st.st_mode)) {
            FileNode *child = createNode(entry->d_name, 0);
            if (child) {
                children[childCount++] = child;
            }
        }
    }
    closedir(dir);

    // 排序子结点
    qsort(children, childCount, sizeof(FileNode *), cmpNode);

    // 建立兄弟链表
    for (int i = 0; i < childCount; i++) {
        if (i > 0) {
            children[i - 1]->nextSibling = children[i];
        }
        if (i == 0) {
            node->firstChild = children[i];
        }
    }

    return node;
}

    // TODO: 实现
    // 步骤提示：
    // 1. opendir 打开目录，失败返回 NULL
    // 2. 从 path 中提取最后的目录名作为当前结点名（注意处理根目录"/"）
    // 3. 创建当前目录结点
    // 4. 循环 readdir，跳过 "." 和 ".."
    // 5. 拼接完整路径，用 stat 判断类型
    // 6. 若是目录，递归调用 buildTree；若是普通文件，调用 createNode
    // 7. 将得到的子结点存入临时数组
    // 8. 关闭目录
    // 9. 对子结点数组排序（调用 qsort 和 cmpNode）
    // 10. 将排序后的子结点链接成兄弟链表（firstChild 指向第一个，后续 nextSibling）
    // 11. 释放临时数组，返回当前目录结点
    // 树形输出（仿 tree 命令）
void printTree(FileNode *node, const char *prefix, int isLast) {
    if (!node) return;

    printf("%s", prefix);
    printf("%s", isLast ? "`-- " : "|-- ");
    printf("%s", node->name);
    if (node->isDir) printf("/");
    printf("\n");

    if (node->firstChild) {
        char newPrefix[1024];
        snprintf(newPrefix, sizeof(newPrefix), "%s%s", prefix, isLast ? "    " : "|   ");

        FileNode *child = node->firstChild;
        while (child) {
            printTree(child, newPrefix, child->nextSibling == NULL);
            child = child->nextSibling;
        }
    }
    // TODO: 实现
    // 步骤提示：
    // 1. 如果 node 为空，返回
    // 2. 输出前缀、分支符号（isLast ? "`-- " : "|-- "）、结点名
    // 3. 如果是目录，输出 "/"
    // 4. 换行
    // 5. 如果没有孩子，返回
    // 6. 遍历孩子链表，对每个孩子：
    //     计算新前缀 = prefix + (isLast ? "    " : "|   ")
    //     判断是否为最后一个孩子
    //     递归调用 printTree
}

// 统计二叉树结点总数
int countNodes(FileNode *root) {
    if (!root) return 0;
    return 1 + countNodes(root->firstChild) + countNodes(root->nextSibling);
}
    // TODO: 实现（递归）



// 统计叶子结点数（firstChild == NULL 的结点）
int countLeaves(FileNode *root) {
    if (!root) return 0;
    if (!root->firstChild) return 1;
    return countLeaves(root->firstChild) + countLeaves(root->nextSibling);
}
    // TODO: 实现（递归）
    // 计算二叉树高度（根深度为1，空树高度为0）
int treeHeight(FileNode *root) {
    if (!root) return 0;
    int childHeight = treeHeight(root->firstChild);
    int siblingHeight = treeHeight(root->nextSibling);
    return 1 + (childHeight > siblingHeight ? childHeight : siblingHeight);
}
    // TODO: 实现（递归）



// 统计目录数和文件数（遍历整棵树）
void countDirFile(FileNode *root, int *dirs, int *files) {
    if (!root) return;
    if (root->isDir) (*dirs)++;
    else (*files)++;

    countDirFile(root->firstChild, dirs, files);
    countDirFile(root->nextSibling, dirs, files);
    // TODO: 实现（递归）
}

// 释放整棵树的内存
void freeTree(FileNode *root) {
     if (!root) return;
    freeTree(root->firstChild);
    freeTree(root->nextSibling);
    free(root->name);
    free(root);
    // TODO: 实现（递归释放左右子树，最后释放当前结点）
}

// 获取当前工作目录的“基本名称”（用于显示根结点名）
char* getBaseName(void) {
    char *cwd = getcwd(NULL, 0);
    if (!cwd) {
        perror("getcwd");
        return NULL;
    }
    char *baseName = strrchr(cwd, '/');
    if (baseName) baseName++;
    else baseName = cwd;

    char *result = strdup(baseName);
    free(cwd);
    return result;
    // TODO: 实现
    // 提示：调用 getcwd(NULL,0) 获取绝对路径，提取最后一个 '/' 之后的部分
    // 注意释放 getcwd 分配的内存
 }

int main(int argc, char *argv[]) {
    char targetPath[1024];
    if (argc >= 2) {
        strncpy(targetPath, argv[1], sizeof(targetPath)-1);
        targetPath[sizeof(targetPath)-1] = '\0';
    } else {
        if (getcwd(targetPath, sizeof(targetPath)) == NULL) {
            perror("getcwd");
            return 1;
        }
    }

    int len = strlen(targetPath);
    if (len > 0 && targetPath[len-1] == '/')
        targetPath[len-1] = '\0';

    struct stat st;
    if (stat(targetPath, &st) != 0) {
        perror("stat");
        return 1;
    }
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "错误: %s 不是目录\n", targetPath);
        return 1;
    }

    FileNode *root = buildTree(targetPath);
    if (!root) {
        fprintf(stderr, "无法构建目录树\n");
        return 1;
    }

    // 输出根目录名
    char *displayName = NULL;
    if (argc >= 2) {
        displayName = root->name;
    } else {
        displayName = getBaseName();
    }
    printf("%s/\n", displayName);
    if (argc < 2) free(displayName);

    FileNode *child = root->firstChild;
    int childCount = 0;
    FileNode *tmp = child;
    while (tmp) { childCount++; tmp = tmp->nextSibling; }
    int idx = 0;
    while (child) {
        int isLast = (++idx == childCount);
        printTree(child, "", isLast);
        child = child->nextSibling;
    }

    int dirs = 0, files = 0;
    countDirFile(root, &dirs, &files);
    printf("\n%d 个目录, %d 个文件\n", dirs, files);
    printf("二叉树结点总数: %d\n", countNodes(root));
    printf("叶子结点数: %d\n", countLeaves(root));
    printf("树的高度: %d\n", treeHeight(root));

    freeTree(root);
    return 0;
}
