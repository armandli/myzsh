#include <jobs/jobs.h>

#include <util/todo.h>

namespace myzsh {

ProcessState Job::state() const {
  // TODO(impl)
  todo("Job::state");
}

int Job::exit_status() const {
  // TODO(impl)
  todo("Job::exit_status");
}

Job& JobTable::add(pid_t pgid, strv text, bool foreground) {
  // TODO(impl): new background jobs become current.
  todo("JobTable::add");
}

bool JobTable::remove(int number) {
  // TODO(impl): fix up current/previous.
  todo("JobTable::remove");
}

opt<s::reference_wrapper<Job>> JobTable::find(int number) {
  // TODO(impl)
  todo("JobTable::find");
}

opt<s::reference_wrapper<Job>> JobTable::find_by_pid(pid_t pid) {
  // TODO(impl)
  todo("JobTable::find_by_pid");
}

opt<int> JobTable::resolve(strv spec) const {
  // TODO(impl): getjob() in Src/jobs.c.
  todo("JobTable::resolve");
}

opt<int> JobTable::current() const {
  // TODO(impl)
  todo("JobTable::current");
}

opt<int> JobTable::previous() const {
  // TODO(impl)
  todo("JobTable::previous");
}

void JobTable::make_current(int number) {
  // TODO(impl)
  todo("JobTable::make_current");
}

opt<int> JobTable::update(pid_t pid, int wait_status) {
  // TODO(impl): WIFSTOPPED -> Stopped, WIFCONTINUED -> Running, else Done.
  todo("JobTable::update");
}

str JobTable::format(const Job& job, bool long_format) const {
  // TODO(impl): printjob() in Src/jobs.c.
  todo("JobTable::format");
}

vec<int> JobTable::numbers() const {
  // TODO(impl)
  todo("JobTable::numbers");
}

void JobTable::prune() {
  // TODO(impl)
  todo("JobTable::prune");
}

str describe_wait_status(int wait_status, ProcessState state) {
  // TODO(impl): signal descriptions as in Src/signames: "interrupt",
  // "terminated", "killed", "hangup", "segmentation fault"...
  todo("describe_wait_status");
}

}  // namespace myzsh
