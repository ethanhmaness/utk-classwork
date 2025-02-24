#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "jrb.h"
#include "dllist.h"
#include "sockettome.h"

// Stores all rooms using Room struct
JRB room_tree;

typedef struct client
{
	char *name;
	FILE *rf, *wf;
} *Client;

typedef struct room
{
	char *name;
	Dllist clients, msg_queue;
	pthread_mutex_t *mutex;
	pthread_cond_t *cv;
} *Room;

// Calls fputs and immediately flushes
void fputsflush(const char *s, FILE *stream)
{
	fputs(s, stream);
	fflush(stream);
}

// Prints a string to all clients of a room
void print_to_room (const char *s, Room room)
{
	Dllist dll_ptr;
	dll_traverse(dll_ptr, room->clients)
	{
		fputsflush(s, ((Client) dll_ptr->val.v)->wf);
	}
}

// Adds a string to the message queue of a room and signals the room's cv
void send_message(char *s, Room room)
{
	pthread_mutex_lock(room->mutex);
	dll_append(room->msg_queue, new_jval_s(s));
	pthread_cond_signal(room->cv);
	pthread_mutex_unlock(room->mutex);
}

// Runs the client threads
void *client_func(void *arg)
{
	Client client = (Client) arg;
	char *inputbuf = calloc(1, sizeof(char) * 1024);
	char *outputbuf = calloc(1, sizeof(char) * 1024);

	JRB jrb_ptr;
	Dllist client_node, dll_ptr;
	Room room;

	// Print all chat rooms in lexicographical order
	fputsflush("Chat Rooms:\n\n", client->wf);
	jrb_traverse(jrb_ptr, room_tree)
	{
		// Build chat room current clients string
		room = ((Room) jrb_ptr->val.v);
		sprintf(outputbuf, "%s:", room->name);

		// Lock room mutex while accessing clients list
		pthread_mutex_lock(room->mutex);
		dll_traverse(dll_ptr, ((Room) jrb_ptr->val.v)->clients)
		{
			strcat(outputbuf, " ");
			strcat(outputbuf, ((Client) dll_ptr->val.v)->name);
		}
		pthread_mutex_unlock(room->mutex);

		strcat(outputbuf, "\n");

		// Print room string
		fputsflush(outputbuf, client->wf);
	}

	// Get client name
	fputsflush("\nEnter your chat name (no spaces):\n", client->wf);
	if (fgets(inputbuf, 1024, client->rf) != NULL)
	{
		// Cut string at newline
		inputbuf[strcspn(inputbuf, "\n")] = '\0';

		// Copy string to client name
		client->name = malloc(sizeof(char) * (strlen(inputbuf) + 1) );
		strcpy(client->name, inputbuf);
	}
	else
	{
		fclose(client->rf);
		fclose(client->wf);
		free(client);
		free(inputbuf);
		free(outputbuf);
		pthread_detach(pthread_self());
		pthread_exit(NULL);
		return 0;
	}

	// Get chat room
	fputsflush("Enter chat room:\n", client->wf);
	if (fgets(inputbuf, 1024, client->rf) != NULL)
	{
		// Cut string at newline
		inputbuf[strcspn(inputbuf, "\n")] = '\0';

		// Find room
		jrb_ptr = jrb_find_str(room_tree, inputbuf);
		room = (Room) jrb_ptr->val.v;

		// Lock room mutex while adding client to list
		pthread_mutex_lock(room->mutex);

		// Add client to room
		dll_append(room->clients, new_jval_v(client));

		// Unlock mutex
		pthread_mutex_unlock(room->mutex);

		// Keep track of client node in list for memory deletion
		client_node = dll_last(room->clients);

		// Send join message
		sprintf(outputbuf, "%s has joined\n", client->name);
		send_message(strcpy(malloc(sizeof(char) * (strlen(outputbuf) + 1)), outputbuf), room);
	}
	else
	{
		fclose(client->rf);
		fclose(client->wf);
		free(client->name);
		free(client);
		free(inputbuf);
		free(outputbuf);
		pthread_detach(pthread_self());
		pthread_exit(NULL);
		return 0;
	}

	// Read input from client
	while (fgets(inputbuf, 1000, client->rf) != NULL)
	{
		// Cut message at newline
		inputbuf[strcspn(inputbuf, "\n")] = '\0';

		// Format with client name
		sprintf(outputbuf, "%s: %s\n", client->name, inputbuf);

		// Send message to room
		send_message(strcpy(malloc(sizeof(char) * (strlen(outputbuf) + 1)), outputbuf), room);
	}

	// Print leave message
	sprintf(outputbuf, "%s has left\n", client->name);
	send_message(strcpy(malloc(sizeof(char) * (strlen(outputbuf) + 1)), outputbuf), room);

	// lock room mutex and remove client
	pthread_mutex_lock(room->mutex);
	dll_delete_node(client_node);
	pthread_mutex_unlock(room->mutex);

	// cleanup files and memory
	fclose(client->rf);
	fclose(client->wf);
	free(client->name);
	free(client);
	free(inputbuf);
	free(outputbuf);
	pthread_detach(pthread_self());
	pthread_exit(NULL);
	return 0;
}

// Runs the room threads
void *room_func(void *arg)
{
	Room room = (Room) arg;
	Dllist dll_ptr;
	char *msg;

	// Initialize mutex
	pthread_mutex_init(room->mutex, NULL);
	pthread_mutex_lock(room->mutex);

	// Wait for signal from clients
	while(pthread_cond_wait(room->cv, room->mutex) == 0)
	{
		// Print all messages in queue
		dll_traverse(dll_ptr, room->msg_queue)
		{
			msg = dll_ptr->val.s;
			print_to_room(msg, room);
		}

		// Empty queue
		while ((dll_ptr = dll_first(room->msg_queue)) != dll_nil(room->msg_queue))
		{
			free(dll_ptr->val.s);
			dll_delete_node(dll_ptr);
		}
	}

	// control should never reach here

	return 0;
}

int main(int argc, char** argv)
{
	// Check args
	if (argc < 3)
	{
		printf("Usage: %s <port> <chat room names...>\n", argv[0]);
		exit(1);
	}

	pthread_t thread_id;

	//init room tree
	room_tree = make_jrb();

	// Populate room_tree
	for (int i = 2; i < argc; i++)
	{
		// Initialize Room struct
		Room room = malloc(sizeof(struct room));
		room->name = argv[i];
		room->clients = new_dllist();
		room->msg_queue = new_dllist();
		room->mutex = malloc(sizeof(pthread_mutex_t));
		room->cv = malloc(sizeof(pthread_cond_t));

		// Insert into tree
		jrb_insert_str(room_tree, room->name, new_jval_v(room));

		// Create room thread
		if (pthread_create(&thread_id, NULL, room_func, room) != 0)
		{
			exit(1);
		}
	}

	// Open socket on provided port number
	int acc_fd, socket_fd, socket_num = atoi(argv[1]);
	
	socket_fd = serve_socket(socket_num);

	// Create client threads on loop
	while ((acc_fd = accept_connection(socket_fd)))
	{
		Client client = malloc(sizeof(struct client));
		client->rf = fdopen(acc_fd, "r");
		client->wf = fdopen(acc_fd, "w");
		pthread_create(&thread_id, NULL, client_func, client);
	}

	// control should never reach here

	return 0;
}
