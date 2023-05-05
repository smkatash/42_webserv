eventSet(struct kevent eventList, int socketDescriptor)
{
	EV_SET(&change_list[0], server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
}


addToEventList(struct kevent eventList, int socketDescriptor)
{
}

// struct kevent {
// 			uintptr_t       ident;          /* identifier for this event */
// 			int16_t         filter;         /* filter for event */
// 			uint16_t        flags;          /* general flags */
// 			uint32_t        fflags;         /* filter-specific flags */
// 			intptr_t        data;           /* filter-specific data */
// 			void            *udata;         /* opaque user data identifier */
// 	};