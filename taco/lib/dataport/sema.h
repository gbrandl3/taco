#define DEADLOCK_WAIT 15
#define OS9_DEADLOCK_WAIT	DEADLOCK_WAIT*256

int define_sema ();
int delete_sema ();
int get_sema ();
int release_sema ();
