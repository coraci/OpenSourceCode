apue/lock/daemon.c                                                                                     664     340      24         2074  5211773771   7412                                                                                                                                                                                                                                                                                                                                                                      #include	<sys/types.h>
#include	<sys/stat.h>
#include	<errno.h>
#include	<fcntl.h>
#include	"ourhdr.h"

#define	PIDFILE		"daemon.pid"

int
main(void)
{
	int		fd, val;
	char	buf[10];

	if ( (fd = open(PIDFILE, O_WRONLY | O_CREAT, FILE_MODE)) < 0)
		err_sys("open error");

			/* try and set a write lock on the entire file */
	if (write_lock(fd, 0, SEEK_SET, 0) < 0)	{
		if (errno == EACCES || errno == EAGAIN)
			exit(0);	/* gracefully exit, daemon is already running */
		else
			err_sys("write_lock error");
	}

			/* truncate to zero length, now that we have the lock */
	if (ftruncate(fd, 0) < 0)
		err_sys("ftruncate error");

			/* and write our process ID */
	sprintf(buf, "%d\n", getpid());
	if (write(fd, buf, strlen(buf)) != strlen(buf))
		err_sys("write error");

			/* set close-on-exec flag for descriptor */
	if ( (val = fcntl(fd, F_GETFD, 0)) < 0)
		err_sys("fcntl F_GETFD error");
	val |= FD_CLOEXEC;
	if (fcntl(fd, F_SETFD, val) < 0)
		err_sys("fcntl F_SETFD error");

	/* leave file open until we terminate: lock will be held */

	/* do whatever ... */

	exit(0);
}
octl(fds, I_PUSH, "ttcompat") < 0) {
		close(fdm);
		close(fds);
		return(-8);
	}

	return(fds);
}
0)
						return(-1);
					newfd = recvfd.fd;	/* new descriptor */
				} else
					newfd = -status;
				nread -= 2;
			}
		}
		if (nread > 0)
			if ((*userfunc)(STDERR_FILENO, buf, nread) != nread)
				return(-1);

		if (status >= 0)	/* final data has arrived */
			return(newfd);	/* descriptor, or -status */
	}
}
(_do_pathconf(name));
}
nk(unix_addr.suapue/lock/deadlock.c                                                                                   664     340      24         1612  5211773771   7712                                                                                                                                                                                                                                                                                                                                                                      #include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	"ourhdr.h"

static void lockabyte(const char *, int, off_t);

int
main(void)
{
	int		fd;
	pid_t	pid;

		/* Create a file and write two bytes to it */
	if ( (fd = creat("templock", FILE_MODE)) < 0)
		err_sys("creat error");
	if (write(fd, "ab", 2) != 2)
		err_sys("write error");

	TELL_WAIT();
	if ( (pid = fork()) < 0)
		err_sys("fork error");

	else if (pid == 0) {			/* child */
		lockabyte("child", fd, 0);
		TELL_PARENT(getppid());
		WAIT_PARENT();
		lockabyte("child", fd, 1);

	} else {						/* parent */
		lockabyte("parent", fd, 1);
		TELL_CHILD(pid);
		WAIT_CHILD();
		lockabyte("parent", fd, 0);
	}
	exit(0);
}

static void
lockabyte(const char *name, int fd, off_t offset)
{
	if (writew_lock(fd, offset, SEEK_SET, 1) < 0)
		err_sys("%s: writew_lock error", name);

	printf("%s: got the lock, byte %d\n", name, offset);
}
D_CLOEXEC;
	if (fcntl(fd, F_SETFD, val) < 0)
		err_sys("fcntl F_SETFD error");

	/* leave file open until we terminateapue/lock/mandatory.c                                                                                  644     340      24         2730  5211773771  10142                                                                                                                                                                                                                                                                                                                                                                      #include	<s