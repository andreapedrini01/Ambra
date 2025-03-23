#ifndef AMBRA_H
#define AMBRA_H

#include "../src/critical_variables.h"

typedef void (task_func_t)(void);
typedef uint32_t task_idx_t;

#define CUR_TASK (curctx->task)
#define TRUE 1
#define FALSE 0

typedef enum {
	READY,
	COMMIT1,
	COMMIT2
} State;

/** @brief manager for the variables */
typedef struct
{
		//Double buffer
		CritVar buffer[2];
		//Signature to know if variables have to be initialized
		AlignedVar signature;
		//Commit bit
		AlignedVar needCommit;
		//Main index
		AlignedVar index;
		//Second index
		AlignedVar newIndex;
		//State variable
		State state ALIGN16;
	
} __align(16) StateManager;

/** @brief Task */
typedef struct task_t {
	/** @brief function address */
	task_func_t *func;
	/** @brief index (only used for showing progress) */
	task_idx_t idx;
	/** @brief helpful for keeping track of state and return task */
	State state ALIGN16;
} task_t;

/** @brief Execution context */
typedef struct _context_t {
	/** @brief indicate whether to jump to commit stage on power failure*/
	uint32_t needCommit ALIGN16;
	/** @brief current running task */
	task_t *task ALIGN16;
} context_t;

extern StateManager manager;
extern context_t * volatile curctx;

/** @brief Function called on every reboot
 *  @details This function usually initializes hardware, such as GPIO
 *           direction. The application must define this function because
 *           different app uses different GPIO.
 */
extern void init(void);

void init_state_manager(void);
void need_commit_buffer(int choice);
void commit_state(void);
void rollback_state(void);
void update_task_state(context_t* context, State newState);
void update_buffer_state(State newState);
void task_prologue(void);
void transition_to(task_t *task);
void write_to_gbuf(uint8_t *data_src, uint8_t *data_dest, size_t var_size); 

/** @brief Internal macro for constructing name of task symbol */
#define TASK_SYM_NAME(func) _task_ ## func

/** @brief Define a task
 *
 *  @param idx      Global task index, zero-based
 *  @param func     Pointer to task function
 *
 */
#define TASK(idx, func) \
__nv task_t TASK_SYM_NAME(func) = { func, idx }; \

/** @brief Declare a task
 *
 *  @param func     Pointer to task function
 *
 */
#define TASK_DEC(func) task_t TASK_SYM_NAME(func)

/** @brief Macro for getting address of task */
#define TASK_REF(func) (&TASK_SYM_NAME(func))

/** @brief First task to run when the application starts
 *  @details Symbol is defined by the ENTRY_TASK macro.
 *           This is not wrapped into a declaration macro, because applications
 *           are not meant to declare tasks -- internal only.
 */
extern task_t TASK_SYM_NAME(_entry_task);

/** @brief Declare the first task of the application
 *  @details This macro defines a function with a special name that is
 *           used to initialize the current task pointer.
 *
 *           This does incur the penalty of an extra task transition, but it
 *           happens only once in application lifetime.
 *
 *           The alternatives are to force the user to define functions
 *           with a special name or to define a task pointer symbol outside
 *           of the library.
 */
#define ENTRY_TASK(task) \
	void _entry_task(void); \
	TASK(0, _entry_task) \
void _entry_task(void) { TRANSITION_TO(task); }

/** @brief Init function prototype
 *  @details We rely on the special name of this symbol to initialize the
 *           current task pointer. The entry function is defined in the user
 *           application through a macro provided by our header.
 */
void _init(void);

/** @brief Declare the function to be called on each boot
 *  @details The same notes apply as for entry task.
 */
#define INIT_FUNC(func) void _init() { func(); }

/**
 *  @brief way to simply rename vars.
 */
#define GLOBAL_SB(type, name, ...) GLOBAL_SB_(type, name, ##__VA_ARGS__, 3, 2)
#define GLOBAL_SB_(type, name, size, n, ...) GLOBAL_SB##n(type, name, size)
#define GLOBAL_SB2(type, name, ...) __nv type name
#define GLOBAL_SB3(type, name, size) __nv type name[size]

/**
 *  @brief way to simply reference manager's vars.
 */
#define GV(type, ...) GV_(type, ##__VA_ARGS__, 2, 1)
#define GV_(type, i, n, ...) GV##n(type, i)
#define GV1(type, ...) manager.type.value
#define GV2(type, i) manager.type[i].value
#define GV_STATE manager.state

//use this to modify your work variable
//to be sure to not modify variables in wrong buffer.
#define GVB(type, ...) GVB_(type, ##__VA_ARGS__, 2, 1)
#define GVB_(type, i, n, ...) GVB##n(type, i)
#define GVB1(type, ...) manager.buffer[1-manager.index.value].type.value
#define GVB2(type, i) manager.buffer[1-manager.index.value].type[i].value

/** @brief Transfer control to the given task.
 *  @param task     Name of the task function
 *  */
#define TRANSITION_TO(task) transition_to(TASK_REF(task))

#endif // AMBRA_H
