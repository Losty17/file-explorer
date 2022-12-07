#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

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
 * Definição básica de uma pilha para utilizar 
 * no sistema de navegação de arquivos.
 * 
 * @param data Texto do nodo atual da pilha.
 * @param next próximo nodo na pilha.
 */
typedef struct stack_t
{
    char *data;
    struct stack_t *next;
} stack_t;

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
/**
 * @brief Remove o nodo especificado. Caso seja uma pasta e tenha filhos, todos serão
 * removidos recursivamente.
 *
 * @param node Nodo a ser removido.
 */
void tree_remove_node(tree_node_t *node);

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
void cmd_list(tree_node_t *current, char *path);
/**
 * @brief Cria um diretório a partir do caminho fornecido.
 *
 * @param node Nodo atual.
 * @param path Caminho do diretório a ser criado.
 */
void cmd_make_directory(tree_node_t *current, char *path);
/**
 * @brief Remove um diretório ou arquivo a partir do caminho fornecido.
 *
 * @param node Nodo atual.
 * @param path Caminho do diretório ou arquivo a ser removido.
 */
void cmd_remove(tree_node_t *current, char *path);
/**
 * @brief Cria um arquivo de texto a partir do caminho fornecido, 
 * fornecendo uma pequena interface para escrever um texto. A cada
 * utilização, o texto antigo é substituído.
 *
 * @param node Nodo atual.
 * @param path Caminho do arquivo a ser criado.
 */
void cmd_nano(tree_node_t *current, char *path);
/**
 * @brief Exibe o texto de um arquivo criado com o nano.
 *
 * @param node Nodo atual.
 * @param path Caminho do arquivo a ser exibido.
 */
void cmd_cat(tree_node_t *current, char *path);

// --- FILE SYSTEM ---++++
/**
 * @brief Inicializa um nodo raiz com uma arquitetura de arquivos
 * semelhante a um sistema operacional UNIX (Linux e derivados).
 */
tree_node_t *fs_init();
/**
 * @brief Retorna o caminho da pasta atual.
 * 
 * @param navigation_data provém as informações necessárias para identificar a pasta atual.
 */
char *fs_get_current_path(navigation_data_t *navigation_data);
/**
 * @brief Converte um caminho de novo nodo em um caminho para encontrar o seu futuro nodo pai.
 * e.g. `/home/user/nova_pasta` -> `/home/user`
 * 
 * @param path caminho a ser alterado.
 */
char *fs_prepare_creation_path(char *path);
/**
 * @brief Recebe o nome do novo arquivo ou diretório a partir do seu caminho.
 * e.g. `/home/user/nova_pasta` -> `nova_pasta`
 * 
 * @param path caminho a ser alterado.
 */
char *fs_prepare_filename(char *path);

// --- STACK ---
/**
 * @brief Inicia uma pilha vazia com o texto fornecido. 
 * Normalmente é iniciada com um `/` para representar a raíz.
 * 
 * @param data texto do primeiro nodo da pilha.
 */
stack_t *stack_init(char *data);
/**
 * @brief Remove o nodo do topo da pilha e o retorna.
 * 
 * @param stack pilha a ser alterada.
 */
stack_t *stack_pop(stack_t **stack);
/**
 * @brief Adiciona um nodo ao topo da pilha.
 * 
 * @param stack pilha a ser alterada.
 * @param data texto do nodo.
 */
void stack_push(stack_t **stack, char *data);
/**
 * @brief Destrói a pilha.
 * 
 * @param stack pilha a ser destruída.
 */
void stack_free(stack_t *stack);

// --- MAIN FUNCTION ---
int main()
{
    tree_node_t *root = fs_init();

    navigation_data_t *navigation_data = (navigation_data_t *)malloc(sizeof(navigation_data_t));
    navigation_data->curr_node = root;
    navigation_data->root = root;
    navigation_data->stack_path = stack_init("/");

    char *opt = (char *)malloc(sizeof(char) * 150);
    do
    {
        printf("sudo@root:%s $ ", fs_get_current_path(navigation_data));
        scanf(" %[^\n]s", opt);
        char *cmd = strtok(opt, " ");
        char *arg = strtok(NULL, " ");

        if (strcmp(cmd, "cat") == 0)
            cmd_cat(navigation_data->curr_node, arg);
        else if (strcmp(cmd, "cd") == 0)
            cmd_change_directory(&navigation_data, arg);
        else if (strcmp(cmd, "clear") == 0)
            system(CLEAR);
        else if (strcmp(cmd, "exit") == 0)
            break;
        else if (strcmp(cmd, "help") == 0)
            printf("cat cd clear exit help ls mkdir nano rm");
        else if (strcmp(cmd, "ls") == 0)
            cmd_list(navigation_data->curr_node, arg);
        else if (strcmp(cmd, "mkdir") == 0)
            cmd_make_directory(navigation_data->curr_node, arg);
        else if (strcmp(cmd, "nano") == 0)
            cmd_nano(navigation_data->curr_node, arg);
        else if (strcmp(cmd, "rm") == 0)
            cmd_remove(navigation_data->curr_node, arg);
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

void cmd_list(tree_node_t *current, char *path)
{
    if (current == NULL)
        return;

    tree_node_t *tmp;

    if (path == NULL)
    {
        tmp = current->child;
    }
    else 
    {
        tmp = tree_find(current, path);

        if (tmp) tmp = tmp->child;
    }

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
        return current;

    if (strlen(path) <= 2)
    {
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
    }

    // Checks if the path starts in root
    if (strncmp(path, "/", strlen("/")) == 0)
    {
        current = tree_find(current, "/");
        path = path + 1;
    }

    if (strcmp(path, "") == 0)
        return current;

    tree_node_t *tmp = current->child;
    char *token = strtok(path, "/");

    while (tmp != NULL)
    {
        if (strcmp(tmp->data.name, token) == 0)
        {
            if (tmp->data.type == _FOLDER)
                return tree_find(tmp, strtok(NULL, "/"));
            else
                return tmp;
        }

        tmp = tmp->sibling;
    }

    return NULL;
}

void cmd_change_directory(navigation_data_t **navigation_data, char *path)
{
    char *path_copy = (char *)malloc(sizeof(char) * strlen(path) + 1);
    strcpy(path_copy, path);

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
    {
        stack_pop(&(*navigation_data)->stack_path);
    }
    else if ((*navigation_data)->curr_node != new_current)
    {
        // Clears the stack if the new path starts on root
        if (strncmp(path_copy, "/", strlen("/")) == 0)
        {
            stack_free((*navigation_data)->stack_path);
            (*navigation_data)->stack_path = stack_init("/");
        }

        char *token = strtok(path_copy, "/");
        while (token != NULL)
        {
            stack_push(&(*navigation_data)->stack_path, token);
            token = strtok(NULL, "/");
        }
    }

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

void cmd_make_directory(tree_node_t *current, char *path)
{
    tree_node_t *target = tree_find(current, fs_prepare_creation_path(path));

    if (target == NULL)
        target = current;
        
    if (target->data.can_write)
        tree_create_node(target, fs_prepare_filename(path), _FOLDER, true, true);
    else
        printf("Permission denied\n");
}

void cmd_nano(tree_node_t *current, char *path)
{
    char *p = fs_prepare_creation_path(path);
    tree_node_t *target = tree_find(current, p);
    
    if (target == NULL)
    {
        target = tree_create_node(current, path, _FILE, true, true);
    }

    if (target->data.type != _FILE)
    {
        printf("Not a file\n");
        return;
    }

    if (!target->data.can_write)
    {
        printf("Permission denied\n");
        return;
    }

    char *content = (char *)malloc(sizeof(char) * 1000);
    char *tmp = (char *)malloc(sizeof(char) * 1000);
    strcpy(content, "");
    strcpy(tmp, "");
    while (strlen(content) < 1000)
    {
        scanf(" %[^\n]s", tmp);
        if (strcmp(tmp, "exit") == 0)
            break;

        strcat(content, tmp);
        strcat(content, "\n");
    }

    target->data.content = content;
}

void cmd_cat(tree_node_t *current, char *path)
{
    tree_node_t *target = tree_find(current, path);
    if (target == NULL)
    {
        printf("File not found\n");
        return;
    }

    if (target->data.type != _FILE)
    {
        printf("Not a file\n");
        return;
    }

    if (!target->data.can_read)
    {
        printf("Permission denied\n");
        return;
    }

    printf("%s", target->data.content);
}

void cmd_remove(tree_node_t *current, char *path)
{
    tree_node_t *target = tree_find(current, path);
    if (target == NULL)
    {
        printf("Not found\n");
        return;
    }

    if (!target->data.can_write)
    {
        printf("Permission denied\n");
        return;
    }

    tree_remove_node(target);
}

void tree_remove_subtree(tree_node_t *node)
{
    if (node->child == NULL)
    {
        free(node);
        return;
    }

    tree_node_t *curr = node->child;
    while (curr != NULL)
    {
        tree_node_t *next = curr->sibling;
        tree_remove_subtree(curr);
        curr = next;
    }
}

void tree_remove_node(tree_node_t *node)
{
    if (node->parent == NULL)
    {
        free(node);
        return;
    }

    tree_node_t *parent = node->parent;
    tree_node_t *prev = NULL;
    tree_node_t *curr = parent->child;

    while (curr != NULL)
    {
        if (curr->sibling == node)
        {
            prev = curr;
            break;
        }

        curr = curr->sibling;
    }

    if (prev) prev->sibling = node->sibling;
    else parent->child = node->sibling;
    
    tree_remove_subtree(node);
}

char *fs_prepare_creation_path(char *path)
{
    // make a substring of the provided path until the last '/'
    char *tmp = (char *)malloc(sizeof(char) * (strlen(path) + 1));
    int last_slash = (int)(strrchr(path, '/') - path);

    int i = 0;
    while (path[i] != '\0')
    {
        if (i == last_slash)
        {
            tmp[i] = '\0';
            break;
        }

        tmp[i] = path[i];
        i++;
    }

    return realloc(tmp, sizeof(char) * (strlen(tmp) + 1));
}

char *fs_prepare_filename(char *path)
{
    char *tmp = (char *)malloc(sizeof(char) * (strlen(path) + 1));
    int last_slash = (int)(strrchr(path, '/') - path);

    int i = last_slash + 1;
    int j = 0;
    while (path[i] != '\0')
    {
        tmp[j] = path[i];
        i++; j++;
    }
    tmp[j] = '\0';

    return realloc(tmp, sizeof(char) * (strlen(tmp) + 1));
}