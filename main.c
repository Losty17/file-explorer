#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct stack_t
{
    char *data;
    struct stack_t *next;
} stack_t;

/**
 * @brief Tipos utilizados para representar os
 * tipos de nodo da árvore.
 */
enum Types
{
    _FILE,
    _FOLDER
};

/**
 * Definição dos dados utilizados em um nodo da árvore.
 *
 * @param type Indica se é uma pasta ou um arquivo.
 * @param name Nome do arquivo ou pasta.
 * @param content Conteúdo do arquivo.
 * @param can_read Permissão de leitura.
 * @param can_write Permissão de escrita.
 */
typedef struct tree_data_t
{
    enum Types type;
    char *name, *content;
    bool can_read, can_write;
    // bool readonly
} tree_data_t;

/**
 * Definição de um nodo da árvore.
 *
 * @param data Dados do nodo.
 * @param parent Nodo pai.
 * @param child Primeiro filho do nodo.
 * @param sibling Próximo nodo irmão.
 * @param depth Profundidade do nodo.
 */
typedef struct tree_node_t
{
    struct tree_data_t data;
    struct tree_node_t *parent, *child, *sibling;
    int depth;
} tree_node_t;

/**
 * Definição do estado do terminal.
 *
 * @param curr_node Nodo atual.
 * @param curr_path Caminho atual.
 */
typedef struct navigation_data_t
{
    tree_node_t *curr_node, *root;
    stack_t *stack_path;
} navigation_data_t;

// --- NODE UTILS ---
/**
 * @brief Cria um novo nodo.
 *
 * @param parent Nodo pai.
 * @param name Nome do nodo.
 * @param type Tipo do nodo.
 */
tree_node_t *tree_create_node(tree_node_t *parent, char *name, enum Types type, bool can_read, bool can_write);
/**
 * @brief Busca um nodo através de um caminho de file system (/pasta/pasta/arquivo).
 *
 * @param current Nodo para iniciar a busca.
 * @param path Caminho para buscar.
 */
tree_node_t *tree_find(tree_node_t *current, char *path);

// --- COMMANDS ---
/**
 * @brief Avança ou retorna o diretório atual.
 *
 * @param navigation_data provém as informações necessárias para alterar o estado do terminal.
 * @param path Caminho a ser alterado.
 */
void cmd_change_directory(navigation_data_t **navigation_data, char *path);
/**
 * @brief Exibe os filhos do nodo informado.
 *
 * @param node Nodo a ser exibido.
 */
void cmd_list(tree_node_t *node);

// --- FILE SYSTEM ---
/**
 * @brief Inicializa um nodo raiz com uma arquitetura de arquivos
 * semelhante a um sistema operacional UNIX (Linux e derivados).
 */
tree_node_t *fs_init();
tree_node_t *fs_create_folder(tree_node_t *parent, char *name);
tree_node_t *fs_create_file(tree_node_t *parent, char *name, char *content, bool can_read, bool can_write);
/**
 * @brief Retorna o caminho da pasta atual.
 * 
 * @param navigation_data provém as informações necessárias para identificar a pasta atual.
 */
char *fs_get_current_path(navigation_data_t *navigation_data);

// --- STACK ---
stack_t *stack_init(char *data);
stack_t *stack_pop(stack_t **stack);
void stack_push(stack_t **stack, char *data);
void stack_free(stack_t *stack);

// --- MAIN FUNCTION ---
int main()
{
    tree_node_t *root = fs_init();

    navigation_data_t *navigation_data = (navigation_data_t *)malloc(sizeof(navigation_data_t));
    navigation_data->curr_node = root;
    navigation_data->root = root;
    navigation_data->stack_path = stack_init("/");

    char *opt = (char *)malloc(sizeof(char) * 100);
    do
    {
        printf("sudo@root:%s $ ", fs_get_current_path(navigation_data));
        scanf(" %[^\n]s", opt);
        char *cmd = strtok(opt, " ");
        char *arg = strtok(NULL, " ");

        if (strcmp(cmd, "ls") == 0)
            cmd_list(navigation_data->curr_node);
        else if (strcmp(cmd, "cd") == 0)
            cmd_change_directory(&navigation_data, arg);
        else if (strcmp(cmd, "help") == 0)
            printf("cd clear exit help ls mkdir nano rm");
        else if (strcmp(cmd, "mkdir") == 0)
            tree_create_node(navigation_data->curr_node, arg, _FOLDER, true, true);
        else if (strcmp(cmd, "nano") == 0)
            tree_create_node(navigation_data->curr_node, arg, _FILE, true, true);
        else if (strcmp(cmd, "exit") == 0)
            break;
        else if (strcmp(cmd, "clear") == 0)
            system("clear");
        else
            printf("Type \"help\" for a list of commands.");

        printf("\n");
    } while (true);

    return printf("\n");
}

char *find_current_path(navigation_data_t *navigation_data)
{
    char *path = (char *)malloc(sizeof(char) * 100);
    tree_node_t *tmp = navigation_data->root;

    while (tmp != navigation_data->curr_node)
    {
        strcat(path, tmp->data.name);
        strcat(path, "/");
        tmp = tmp->parent;
    }

    return path;
}

char *fs_get_current_path(navigation_data_t *navigation_data)
{
    char *path = (char *)malloc(sizeof(char) * 100);
    stack_t *token = navigation_data->stack_path;
    stack_t *tmp = stack_init(token->data);

    while (token)
    {
        token = token->next;
        if (token)
            stack_push(&tmp, token->data);
    }

    token = tmp;

    while (tmp)
    {
        strcat(path, tmp->data);
        if (tmp->next && strcmp(path, "/") != 0)
            strcat(path, "/");
        tmp = tmp->next;
    }

    stack_free(token);

    return path;
}

void cmd_list(tree_node_t *node)
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

tree_node_t *tree_create_node(tree_node_t *parent, char *name, enum Types type, bool can_read, bool can_write)
{
    tree_node_t *node = malloc(sizeof(tree_node_t));
    char *node_name = (char *)malloc(sizeof(char) * 100);
    node->data.name = strcpy(node_name, name);
    node->data.type = type;
    node->data.content = "";
    node->data.can_read = can_read;
    node->data.can_write = can_write;
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

tree_node_t *tree_find(tree_node_t *current, char *path)
{
    if (current == NULL || path == NULL)
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
        current = tree_find(current, "/");
    }

    if (strcmp(path, "") == 0 || strcmp(path, current->data.name) == 0)
        return current;

    tree_node_t *tmp = current->child;

    while (tmp != NULL)
    {
        if (strcmp(tmp->data.name, path) == 0)
            return tmp;

        tmp = tmp->sibling;
    }

    return NULL;
}

void cmd_change_directory(navigation_data_t **navigation_data, char *path)
{
    tree_node_t *new_current = tree_find((*navigation_data)->curr_node, path);

    if (new_current == NULL)
    {
        printf("Directory not found\n");
        return;
    }

    if (new_current->data.type == _FILE)
    {
        printf("Not a directory\n");
        return;
    }

    if ((*navigation_data)->curr_node->parent == new_current)
        stack_pop(&(*navigation_data)->stack_path);
    else if ((*navigation_data)->curr_node != new_current)
        stack_push(&(*navigation_data)->stack_path, path);

    (*navigation_data)->curr_node = new_current;
}

tree_node_t *fs_init()
{
    tree_node_t *root = tree_create_node(NULL, "root", _FOLDER, true, true);

    tree_node_t *home = tree_create_node(root, "home", _FOLDER, true, true);
    tree_node_t *etc = tree_create_node(root, "etc", _FOLDER, true, true);
    tree_node_t *bin = tree_create_node(root, "bin", _FOLDER, true, true);
    tree_node_t *usr = tree_create_node(root, "usr", _FOLDER, true, true);
    tree_node_t *var = tree_create_node(root, "var", _FOLDER, true, true);

    tree_create_node(home, "user1", _FOLDER, true, true);
    tree_create_node(home, "user2", _FOLDER, true, true);
    tree_create_node(home, "user3", _FOLDER, true, true);

    tree_create_node(etc, "passwd", _FILE, true, true);
    tree_create_node(etc, "shadow", _FILE, true, true);
    tree_create_node(etc, "group", _FILE, true, true);

    tree_create_node(bin, "ls", _FILE, true, true);
    tree_create_node(bin, "cd", _FILE, true, true);
    tree_create_node(bin, "mkdir", _FILE, true, true);

    tree_create_node(usr, "local", _FOLDER, true, true);
    tree_create_node(usr, "share", _FOLDER, true, true);

    tree_create_node(var, "log", _FOLDER, true, true);
    tree_create_node(var, "tmp", _FOLDER, true, true);

    return root;
}

stack_t *stack_init(char *data)
{
    stack_t *stack = (stack_t *)malloc(sizeof(stack_t));
    stack->data = data;
    stack->next = NULL;
    return stack;
}

void stack_push(stack_t **stack, char *data)
{
    stack_t *new_data = (stack_t *)malloc(sizeof(stack_t));
    new_data->data = (char *)malloc(sizeof(char) * (strlen(data) + 1));
    strcpy(new_data->data, data);

    new_data->next = *stack;
    *stack = new_data;
}

stack_t *stack_pop(stack_t **stack)
{
    if (!stack)
        return NULL;

    stack_t *tmp = *stack;
    *stack = tmp->next;
    return tmp;
}

void stack_free(stack_t *stack)
{
    stack_t *top = stack;
    while (top != NULL)
    {
        stack_t *next = top->next;
        free(top);
        top = next;
    }
    // free(stack);
}

tree_node_t *fs_create_folder(tree_node_t *parent, char *name)
{

}

tree_node_t *fs_create_file(tree_node_t *parent, char *name, char *content, bool can_read, bool can_write);