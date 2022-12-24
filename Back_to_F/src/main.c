#include "../include/include.h"

// Pourquoi refaire un ft_putchar alors que tu l'as déjà fait dans ton libft ?
void ft_putchar(char c)
{
    write(1, &c, 1);
}

unsigned int	starter_env(char *s)
{
	unsigned int	i = 0;
	while (s[i] && s[i] != '=')
		i++;
	return (i);
}

// Quasiment parfaitement propre (check malloccc)
t_env *get_env(char **env)
{
    int i = 0;
    t_env *list = NULL;
    
    list = malloc(sizeof(t_env)* 100);
    if (!list)
    {
        printf("malloc error\n");
        exit(-127);
    }
    while (env[i])
    {
        list[i].name = ft_strsub(env[i], 0, ft_strchr(env[i], '=') - env[i]);
        list[i].content =  ft_strsub(env[i], starter_env(env[i]), ft_strlen(env[i]) - (ft_strchr(env[i], '=') - env[i]));
        i++;
    }
    return (list);
}

// Dans le directory lexer de préférence
// t_list *lexer(t_list *str)
// {
//     t_list *list;
//     char **tab;
// 	char *tp = (char *)str->content;
//     int i = 0;
//     t_list *tmp = malloc(sizeof(t_list) * 100); // Check tes mallocs mon coeuuurrrrr
//     if (!tmp)
//     {
//         printf("malloc error\n");
//         exit(-127);
//     }
//     tab = ft_split(tp);
//     while (tab[i])
//     {
//         list = ft_lstnew(tab[i]);
//         list->next = tmp;
//         tmp = list;
//         i++;
//     }
// 	//printf("TAB VA SUIVRE---->\n");
// 	//afftab(tab);
//     afflist(list, "Lexer");
//     printf("je vois avant tokenizateur\n");
//     tokenizateur(list);
//     //printf("sortie\n");
//     return (list);
// }

/* by mathieu
Le soucis c'est que tu fais un list->next = tmp et apparament ca faisait un peu n'importe quoi
Donc vaut mieux utiliser une fonction que tu as faites comme ft_lstadd qui elle meme te rajoute une node a la fin
ensuite a chaque fois tu realloues list donc tu auras des leaks alors que tu peux directement donner en argument ft_lstnew 
a ton ft_lstadd, ensuite ton tab recupere les espaces. 
*/

int check_name(char *str, t_list *name)
{
    while (name)
    {
        if (ft_strcmp(str, (char *)name->content) == 0)
            return (1);
        name = name->content;
    }
    return (0);
}

int closed_elem(char *str, char c)
{
	int i = 0;
	// int	start = 0;
	// int	end = ft_strlen(str) - 1;
	int flag = 0;
	int counter = 0;
	char *start = ft_strchr(str, c);
	if (start == NULL)
		return (1);
	while (start[i])
	{
		if (counter == 1 && start[i] == c)
		{
			counter = 0;
			flag = 0;
		}
		if (flag == 1 && start[i] == c)
		{
			counter = 1;
			flag = 0;
		}
		if (start[i] == c && counter != 1)
			flag = 1;
		i++;
	}
	return (counter);
}

int checking_closed(char *tp)
{
	if (!tp)
		return (-2);
	int val_test = closed_elem(tp, '"');
	if (val_test == 0)
		return (0);
	val_test = closed_elem(tp, '\'');
	if (val_test == 0)
		return (-1);
	return (val_test);
}

t_list *lexer(t_list *str)
{
    t_list *list;
    int i;
    char **tab = NULL;

    i = 0;
    list = NULL;
	// printf("tab[i] == %s\n", tab[i])
	printf("ui\n");
    tab = ft_split((char *)str->content);
    while (tab[i])
    {
        ft_lstadd(&list, ft_lstnew(tab[i]));
        i++;
    }
    afflist(list, "Lexer");
    tokenizateur(list);
    return (list);
}

void afftoken(t_token *list, char *name)
{
    (void)name;
    while (list)
    {
        // printf("		--------------------\n\
		// | Affichage %s  |\n\
		// ---------------------\n\
		// 	%s\n", name, list->content);
        printf("\e[0;31mToken : [%s]\e[0m", list->value);
        printf("\e[0;31mToken : [%d]\n\e[0m", list->t_type);
        list = list->next;
    }
}

int	token_size()
{
	int	i = 0;

	if (g_data.token)
	{
		while (g_data.token)
		{
			i++;
			g_data.token = g_data.token->next;
		}
	}
	return (i);
}

void	checking_solo_name(t_token *tmp)
{
	if (token_size() == 0)
	{
		if (tmp->t_type == 4 || tmp->t_type == 5 || tmp->t_type == 6 
				||tmp->t_type == 1 || tmp->t_type == 2 )
		{
			if (ft_strlen(tmp->value) < 2)
			{
				if (tmp->t_type == 6)
					printf("Us_Bash: $: command not found\n");
				else if (tmp->t_type == 1 || tmp->t_type == 2)
					printf("Us_Bash: syntax error near unexpected token 'newline'\n");
				return ;
			}
		}
		if (tmp->t_type == 6)
		{
			tmp->value = search_env_content(g_data.env, tmp->value);
			printf("NOUVELLE VALEUR TMP->VALUE == %s\n", tmp->value);
		}
	}
}

void	*solo_command()
{
	//CRASH SI LE SEUL TOKEN RECU EST UNE REDIR RREDIR PIPE OU OPTION
	t_token	*tmp;

	tmp = g_data.token;
	if (token_size() == 1 && tmp->t_type != 3)
	{
		if (tmp->t_type == 0 || tmp->t_type == 7)
		{
			if (tmp->t_type == 0)
				printf("Us_Bash: syntax error near unexpected token '|'\n");
			// else if (tmp->t_type == 1 || tmp->t_type == 2)
			// 	printf("Us_Bash: syntax error near unexpected token 'newline'\n");
			return (NULL);
		}
		checking_solo_name(tmp);
		g_data.token = tmp;
	}
	else
	{
		printf("Liste de tokens superieur à 1 ou seul un Token_Word\n");
	}
	// SI SEUL TOKEN EST UN WORD, PEUT ETRE UN FILE OU UNE COMMANDE (CMD PEUT ETRE EXEC SEULE COMME ECHO)
		// DOLLAR PEUT AUSSI, DQUOTE SQUOTE AUSSI 
	return ((void *)tmp);
}

void	manage_double()
{
	t_token	*tmp;

	tmp = g_data.token;
	while (tmp)
	{
		
	}
}

void	*parsor(t_list *str)
{
	// t_list	*list = NULL;
	// int		i = 0;
	
	afftoken(g_data.token, "G_DATA_TOKEN");
	afflist(str, "PARSOR");
	//REGLE NUMERO UNE, CHECKER L'ORDRE CHRONO DES TOKENS, ET INSANITY DANS LORGANISATION
	if (solo_command() == NULL)
	{
		return (NULL);
	}
		// printf("g_data'token == %s\n", g_data.token->value);
	
	//GESTION DUN DOUBLE PIPE, DOUBLE OPTION , DOUBLE REDIR OU DOUBLE RREDIR DAFFILES OU SEPARES DUN ESPACE
	// manage_double();

	// SI UN TOKEN WORD OK, SI UN Token_Pipe, UN Token_Option, UN Token_Redir, OU UN Token_Rredir, sans rien, ERROR
		// NE PEUT AVOIR QUE LES TOKEN WORD, DQUOTE/SQUOTE, DOLLARS EN SOLO, les autres need something
	// REGLE NUMERO DEUX, CHECKER SI CHAQUE Token_Word EST UNE COMMANDE OU UN FICHIER D'ENTREE OU SORTIE
		//Reattrib une 
	// return (str);
	return (NULL);
}

void	delete_token(t_token **l)
{
	t_token	*tmp;

	tmp = (*l);
	while ((*l) != NULL)
	{
		tmp = (*l);
		(*l) = (*l)->next;
		free(tmp);
	}
	free(*l);
}

int main(int ac, char **av, char **env)
{
	t_list	*history = NULL;
    t_list *str = NULL;
	history = ft_lstnew(NULL);
    g_data.env = get_env(env);
    (void)ac;
    (void)av;
    //int i = 0;
	
    // BIBOUUUUUUUUUUUUUU
    //
	// while (list[i].name)
	// {
	// 	printf("NAME = %s && CONTENT = %s\n", list[i].name, list[i].content);
	// 	i++;
	// }
    while (42)
	{
		// g_data.token = (t_token *)malloc(sizeof(t_token));
	 	ft_putstr("minishell>");
        str = ft_lstnew(get_next_line(0));
		ft_lstadd(&history, str);
		if (checking_closed((char *)str->content) != 0 && checking_closed((char *)str->content) != -1)
 	    {  
			str = lexer(str);
			parsor(str);
		}
		delete_token(&g_data.token);
	}
    return (0);
}