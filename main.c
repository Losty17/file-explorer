#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

typedef struct navigation_data_t
{
    tree_node_t *current;
} navigation_data_t;

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
        current = find_by_path(current, "/", 0);

    char *token = strtok(path, "/");

    if (strcmp(token, current->data.name) == 0)
    {
        token = strtok(NULL, "/");

        if (token == NULL)
            return current;

        char *new_path = malloc(sizeof(char) * strlen(path));
        while (token)
        {
            strcat(new_path, token);
            strcat(new_path, "/");
            token = strtok(NULL, "/");
        }

        return find_by_path(current->child, new_path, depth + 1);
    }
    else
    {
        char *new_path = malloc(sizeof(char) * strlen(path));
        while (token)
        {
            strcat(new_path, token);
            strcat(new_path, "/");
            token = strtok(NULL, "/");
        }

        return find_by_path(current->child, new_path, depth + 1);
    }
}

int main()
{
    tree_node_t *root = create_node(NULL, "root", FOLDER);
    tree_node_t *current = root;

    create_node(root, "home", FOLDER);
    create_node(root, "etc", FOLDER);
    create_node(root, "bin", FOLDER);
    create_node(root, "usr", FOLDER);

    create_node(find_by_path(root, "/home", 0), "user", FOLDER);
    create_node(find_by_path(root, "/home", 0), "guest", FOLDER);

    printf("%s", find_by_path(find_by_path(root, "home", 0), "user", 0)->data.name);

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
    //         create_node(root, arg, FOLDER);
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