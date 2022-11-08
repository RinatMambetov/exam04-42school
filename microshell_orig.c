#include <string.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct s_cmd
{
	int		pipe[2];
	int		type[2];
	int		prev_pipe;
	char	**args;
}	t_cmd;

int		ft_strlen(char *str)
{
	int		len;

	len = 0;
	while (str[len])
		len++;
	return (len);
}

void	ft_fatal(void)
{
	write(2, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(1);
}

int		ft_list_end(t_cmd *list, int start, char **argv)
{
	int		end;
	int		len;
	int		semicolon_counter;

	semicolon_counter = 0;
	end = start;
	while (argv[end] && strcmp(argv[end], "|") && strcmp(argv[end], ";"))
		end++;
	if (argv[end] && strcmp(argv[end], ";") == 0)
		while (strcmp(argv[end + 1], ";") == 0)
		{
			end++;
			semicolon_counter++;
		}
	len = end - start;
	if (len > 0)
	{
		list->args = &argv[start];
		list->type[0] = list->type[1];
		if (argv[end] && strcmp(argv[end], "|") == 0)
			list->type[1] = 1;
		else
			list->type[1] = 0;
		argv[end] = NULL;
		argv[end - semicolon_counter] = NULL;
		list->prev_pipe = list->pipe[0];
	}
	return (end);
}

void	ft_exec_cmd(t_cmd *list, char **env)
{
	pid_t	pid;
	int		ex;

	if ((list->type[0] == 1 || list->type[1] == 1) && pipe(list->pipe) == -1)
		ft_fatal();
	if ((pid = fork()) == -1)
		ft_fatal();
	if (pid == 0)
	{
		if (list->type[0] == 1 && dup2(list->prev_pipe, 0) == -1)
			ft_fatal();
		if (list->type[1] == 1 && dup2(list->pipe[1], 1) == -1)
			ft_fatal();
		if ((ex = execve(list->args[0], list->args, env)) == -1)
		{
			write(2, "error: cannot execute ", ft_strlen("error: cannot execute "));
			write(2, list->args[0], ft_strlen(list->args[0]));
			write(2, "\n", 1);
		}
		exit(ex);
	}
	else
	{
		waitpid(pid, NULL, 0);
		if (list->type[0] == 1 || list->type[1] == 1)
		{
			close(list->pipe[1]);
			if (list->type[1] != 1)
				close(list->pipe[0]);
		}
		if (list->type[0] == 1)
			close(list->prev_pipe);
	}
}

int		main(int argc, char **argv, char **env)
{
	t_cmd	list;
	int		i;
	int		start;

	i = 1;
	while (i < argc)
	{
		start = i;
		i = ft_list_end(&list, start, argv);
		if (strcmp(argv[start], "cd") == 0)
		{
			if (i - start != 2)
				write(2, "error: cd: bad arguments\n", ft_strlen("error: cd: bad arguments\n"));
			else if (chdir(argv[start + 1]) == -1)
			{
				write(2, "error: cd: cannot change directory to ", ft_strlen("error: cd: cannot change directory to "));
				write(2, argv[start + 1], ft_strlen(argv[start + 1]));
				write(2, "\n", 1);
			}
		}
		else
			ft_exec_cmd(&list, env);
		i++;
	}
	return (0);
}
