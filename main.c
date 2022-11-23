#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Types
{
    _FILE,
    FOLDER
};

typedef struct tree_data_t
{
    enum Types type;
    char *name, *content;
} tree_data_t;

typedef struct tree_node_t 
{
    struct tree_data_t data;
    struct tree_node_t *parent, *child, *sibling;
    int depth;
} tree_node_t;

void list_cmd(tree_node_t *node)
{
    if (node == NULL)
        return;

    tree_node_t *tmp = node->child;

    while (tmp != NULL)
    {
        printf("%s\t", tmp->data.name);
        tmp = tmp->sibling;
    }
}

tree_node_t *create_node(tree_node_t *parent, char *name, enum Types type)
{
    tree_node_t *node = malloc(sizeof(tree_node_t));
    node->data.name = name;
    node->data.type = type;
    node->parent = parent;
    node->child = NULL;
    node->sibling = NULL;
    node->depth = parent ? parent->depth + 1 : 0;

    if (parent)
    {
        if (parent->child == NULL)
        {
            parent->child = node;
        }
        else
        {
            tree_node_t *sibling = parent->child;
            
            while (sibling->sibling != NULL)
            {
                sibling = sibling->sibling;
            }
            
            sibling->sibling = node;
        }
    }

    return node;
}

int main()
{
    tree_node_t *root = create_node(NULL, "root", FOLDER);

    tree_node_t *folder1 = create_node(root, "folder1", FOLDER);
    tree_node_t *folder2 = create_node(root, "folder2", FOLDER);
    tree_node_t *folder3 = create_node(root, "folder3", FOLDER);

    tree_node_t *file1 = create_node(folder1, "file1", _FILE);
    tree_node_t *file2 = create_node(folder1, "file2", _FILE);
    tree_node_t *file3 = create_node(folder1, "file3", _FILE);

    char *opt = (char *)malloc(sizeof(char) * 20);
    do
    {
        printf("sudo@root:~$ ");
        scanf("%s", opt);

        if (strcmp(opt, "ls") == 0)
        {
            list_cmd(root);
        }
        else if (strcmp(opt, "help") == 0)
        {
            printf("cd exit help ls mkdir nano rm");
        }

        printf("\n");
    } while (strcmp(opt, "exit") != 0);
    

    return printf("\n");
}