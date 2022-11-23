#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum Types
{
    _FILE,
    _FOLDER
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

typedef struct navigation_data_t
{
    char *path;
} navigation_data_t;

char *get_new_path(char *path, int depth)
{
    char *ptr = strtok(path, "/");

    depth++;
    while (depth--)
        ptr = strtok(NULL, "/");
    
    char *new_path = malloc(sizeof(char) * strlen(path));

    while (ptr)
    {
        strcat(new_path, ptr);
        strcat(new_path, "/");
        ptr = strtok(NULL, "/");
    }

    return new_path;
}

void print_children(tree_node_t *node)
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

tree_node_t *find_by_path(tree_node_t *current, char *path, int depth)
{
    if (current == NULL)
        return NULL;

    // Returns the folder above
    if (strcmp(path, "..") == 0)
        return current->parent;

    // Returns the current folder
    if (strcmp(path, ".") == 0)
        return current;

    // Returns the root folder
    if (strcmp(path, "/") == 0)
    {
        while (current->parent != NULL)
        {
            current = current->parent;
        }

        return current;
    }

    // Checks if the path starts in root
    if (strncmp(path, "/", strlen("/")) == 0)
    {
        current = find_by_path(current, "/", 0);
        depth = 0;
    }

    char *current_name = strtok(path, "/");
    char *new_path = get_new_path(path, depth);

    if (strcmp(current_name, current->data.name) == 0 && !new_path)
        return current;

    tree_node_t *tmp = current->child;

    while (tmp != NULL)
    {
        if (strcmp(tmp->data.name, current_name) == 0)
        {
            return find_by_path(tmp, new_path, depth - 1);
        }

        tmp = tmp->sibling;
    }

    return tmp;
}


int main()
{
    tree_node_t *root = create_node(NULL, "root", _FOLDER);
    // tree_node_t *current = root;

    tree_node_t *home = create_node(root, "home", _FOLDER);
    create_node(root, "etc", _FOLDER);
    create_node(root, "bin", _FOLDER);
    create_node(root, "usr", _FOLDER);

    create_node(home, "user", _FOLDER);
    create_node(home, "root", _FOLDER);

    char a[] = "/home/user";
    
    printf("%s", find_by_path(root, a, 0)->data.name);

    // char *opt = (char *)malloc(sizeof(char) * 100);
    // do
    // {
    //     printf("sudo@root:~$ ");
    //     scanf(" %[^\n]s", opt);
    //     char *cmd = strtok(opt, " ");
    //     char *arg = strtok(NULL, " ");

    //     if (strcmp(cmd, "ls") == 0)
    //     {
    //         if (arg)
    //         {
    //             tree_node_t *node = find_by_path(current, arg);
    //             print_children(node);
    //         }
    //         else
    //         {
    //             print_children(current);
    //         }
    //     }
    //     else if (strcmp(cmd, "help") == 0)
    //     {
    //         printf("cd exit help ls mkdir nano rm");
    //     }
    //     else if (strcmp(cmd, "mkdir") == 0)
    //     {
    //         create_node(root, arg, _FOLDER);
    //     }
    //     else if (strcmp(cmd, "nano") == 0)
    //     {
    //         create_node(root, arg, _FILE);
    //     }
    //     else if (strcmp(cmd, "exit") == 0)
    //     {
    //         break;
    //     }
    //     else
    //     {
    //         printf("Type \"help\" for a list of commands.");
    //     }

    //     printf("\n");
    // } while (true);
    

    // return printf("\n");
}