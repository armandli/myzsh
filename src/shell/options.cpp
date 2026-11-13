#include <shell/options.h>

#include <array>

#include <util/todo.h>

namespace myzsh {

namespace {

// Generated from Src/options.c in the zsh source tree.
constexpr auto OPTION_TABLE = s::to_array<OptionInfo>({
  {Option::Aliases, "aliases", OF_EMULATE | OF_ALL},
  {Option::Aliasfuncdef, "aliasfuncdef", OF_EMULATE | OF_BOURNE},
  {Option::Allexport, "allexport", OF_EMULATE},
  {Option::Alwayslastprompt, "alwayslastprompt", OF_ALL},
  {Option::Alwaystoend, "alwaystoend", 0},
  {Option::Appendcreate, "appendcreate", OF_EMULATE | OF_BOURNE},
  {Option::Appendhistory, "appendhistory", OF_ALL},
  {Option::Autocd, "autocd", OF_EMULATE},
  {Option::Autocontinue, "autocontinue", 0},
  {Option::Autolist, "autolist", OF_ALL},
  {Option::Automenu, "automenu", OF_ALL},
  {Option::Autonamedirs, "autonamedirs", 0},
  {Option::Autoparamkeys, "autoparamkeys", OF_ALL},
  {Option::Autoparamslash, "autoparamslash", OF_ALL},
  {Option::Autopushd, "autopushd", 0},
  {Option::Autoremoveslash, "autoremoveslash", OF_ALL},
  {Option::Autoresume, "autoresume", 0},
  {Option::Badpattern, "badpattern", OF_EMULATE | OF_NON_BOURNE},
  {Option::Banghist, "banghist", OF_NON_BOURNE},
  {Option::Bareglobqual, "bareglobqual", OF_EMULATE | OF_ZSH},
  {Option::Bashautolist, "bashautolist", 0},
  {Option::Bashrematch, "bashrematch", 0},
  {Option::Beep, "beep", OF_ALL},
  {Option::Bgnice, "bgnice", OF_EMULATE | OF_NON_BOURNE},
  {Option::Braceccl, "braceccl", OF_EMULATE},
  {Option::Bsdecho, "bsdecho", OF_EMULATE | OF_SH},
  {Option::Caseglob, "caseglob", OF_ALL},
  {Option::Casematch, "casematch", OF_ALL},
  {Option::Casepaths, "casepaths", 0},
  {Option::Cbases, "cbases", 0},
  {Option::Cprecedences, "cprecedences", OF_EMULATE | OF_NON_ZSH},
  {Option::Cdablevars, "cdablevars", OF_EMULATE},
  {Option::Cdsilent, "cdsilent", 0},
  {Option::Chasedots, "chasedots", OF_EMULATE},
  {Option::Chaselinks, "chaselinks", OF_EMULATE},
  {Option::Checkjobs, "checkjobs", OF_EMULATE | OF_ZSH},
  {Option::Checkrunningjobs, "checkrunningjobs", OF_EMULATE | OF_ZSH},
  {Option::Clobber, "clobber", OF_EMULATE | OF_ALL},
  {Option::Clobberempty, "clobberempty", 0},
  {Option::Combiningchars, "combiningchars", 0},
  {Option::Completealiases, "completealiases", 0},
  {Option::Completeinword, "completeinword", 0},
  {Option::Continueonerror, "continueonerror", 0},
  {Option::Correct, "correct", 0},
  {Option::Correctall, "correctall", 0},
  {Option::Cshjunkiehistory, "cshjunkiehistory", OF_EMULATE | OF_CSH},
  {Option::Cshjunkieloops, "cshjunkieloops", OF_EMULATE | OF_CSH},
  {Option::Cshjunkiequotes, "cshjunkiequotes", OF_EMULATE | OF_CSH},
  {Option::Cshnullcmd, "cshnullcmd", OF_EMULATE | OF_CSH},
  {Option::Cshnullglob, "cshnullglob", OF_EMULATE | OF_CSH},
  {Option::Debugbeforecmd, "debugbeforecmd", OF_ALL},
  {Option::Emacs, "emacs", 0},
  {Option::Equals, "equals", OF_EMULATE | OF_ZSH},
  {Option::Errexit, "errexit", OF_EMULATE},
  {Option::Errreturn, "errreturn", OF_EMULATE},
  {Option::Exec, "exec", OF_ALL},
  {Option::Extendedglob, "extendedglob", OF_EMULATE},
  {Option::Extendedhistory, "extendedhistory", OF_CSH},
  {Option::Evallineno, "evallineno", OF_EMULATE | OF_ZSH},
  {Option::Flowcontrol, "flowcontrol", OF_ALL},
  {Option::Forcefloat, "forcefloat", 0},
  {Option::Functionargzero, "functionargzero", OF_EMULATE | OF_NON_BOURNE},
  {Option::Glob, "glob", OF_EMULATE | OF_ALL},
  {Option::Globalexport, "globalexport", OF_EMULATE | OF_ZSH},
  {Option::Globalrcs, "globalrcs", OF_ALL},
  {Option::Globassign, "globassign", OF_EMULATE | OF_CSH},
  {Option::Globcomplete, "globcomplete", 0},
  {Option::Globdots, "globdots", OF_EMULATE},
  {Option::Globstarshort, "globstarshort", OF_EMULATE},
  {Option::Globsubst, "globsubst", OF_EMULATE | OF_NON_ZSH},
  {Option::Hashcmds, "hashcmds", OF_ALL},
  {Option::Hashdirs, "hashdirs", OF_ALL},
  {Option::Hashexecutablesonly, "hashexecutablesonly", 0},
  {Option::Hashlistall, "hashlistall", OF_ALL},
  {Option::Histallowclobber, "histallowclobber", 0},
  {Option::Histbeep, "histbeep", OF_ALL},
  {Option::Histexpiredupsfirst, "histexpiredupsfirst", 0},
  {Option::Histfcntllock, "histfcntllock", 0},
  {Option::Histfindnodups, "histfindnodups", 0},
  {Option::Histignorealldups, "histignorealldups", 0},
  {Option::Histignoredups, "histignoredups", 0},
  {Option::Histignorespace, "histignorespace", 0},
  {Option::Histlexwords, "histlexwords", 0},
  {Option::Histnofunctions, "histnofunctions", 0},
  {Option::Histnostore, "histnostore", 0},
  {Option::Histsubstpattern, "histsubstpattern", OF_EMULATE},
  {Option::Histreduceblanks, "histreduceblanks", 0},
  {Option::Histsavebycopy, "histsavebycopy", OF_ALL},
  {Option::Histsavenodups, "histsavenodups", 0},
  {Option::Histverify, "histverify", 0},
  {Option::Hup, "hup", OF_EMULATE | OF_ZSH},
  {Option::Ignorebraces, "ignorebraces", OF_EMULATE | OF_SH},
  {Option::Ignoreclosebraces, "ignoreclosebraces", OF_EMULATE},
  {Option::Ignoreeof, "ignoreeof", 0},
  {Option::Incappendhistory, "incappendhistory", 0},
  {Option::Incappendhistorytime, "incappendhistorytime", 0},
  {Option::Interactive, "interactive", OF_SPECIAL},
  {Option::Interactivecomments, "interactivecomments", OF_BOURNE},
  {Option::Ksharrays, "ksharrays", OF_EMULATE | OF_BOURNE},
  {Option::Kshautoload, "kshautoload", OF_EMULATE | OF_BOURNE},
  {Option::Kshglob, "kshglob", OF_EMULATE | OF_KSH},
  {Option::Kshoptionprint, "kshoptionprint", OF_EMULATE | OF_KSH},
  {Option::Kshtypeset, "kshtypeset", 0},
  {Option::Kshzerosubscript, "kshzerosubscript", 0},
  {Option::Listambiguous, "listambiguous", OF_ALL},
  {Option::Listbeep, "listbeep", OF_ALL},
  {Option::Listpacked, "listpacked", 0},
  {Option::Listrowsfirst, "listrowsfirst", 0},
  {Option::Listtypes, "listtypes", OF_ALL},
  {Option::Localoptions, "localoptions", OF_EMULATE | OF_KSH},
  {Option::Localloops, "localloops", OF_EMULATE},
  {Option::Localpatterns, "localpatterns", OF_EMULATE},
  {Option::Localtraps, "localtraps", OF_EMULATE | OF_KSH},
  {Option::Login, "login", OF_SPECIAL},
  {Option::Longlistjobs, "longlistjobs", 0},
  {Option::Magicequalsubst, "magicequalsubst", OF_EMULATE},
  {Option::Mailwarning, "mailwarning", 0},
  {Option::Markdirs, "markdirs", 0},
  {Option::Menucomplete, "menucomplete", 0},
  {Option::Monitor, "monitor", OF_SPECIAL},
  {Option::Multifuncdef, "multifuncdef", OF_EMULATE | OF_ZSH},
  {Option::Multios, "multios", OF_EMULATE | OF_ZSH},
  {Option::Nomatch, "nomatch", OF_EMULATE | OF_NON_BOURNE},
  {Option::Notify, "notify", OF_ZSH},
  {Option::Nullglob, "nullglob", OF_EMULATE},
  {Option::Numericglobsort, "numericglobsort", OF_EMULATE},
  {Option::Octalzeroes, "octalzeroes", OF_EMULATE | OF_SH},
  {Option::Overstrike, "overstrike", 0},
  {Option::Pathdirs, "pathdirs", OF_EMULATE},
  {Option::Pathscript, "pathscript", OF_EMULATE | OF_BOURNE},
  {Option::Pipefail, "pipefail", OF_EMULATE},
  {Option::Posixaliases, "posixaliases", OF_EMULATE | OF_BOURNE},
  {Option::Posixargzero, "posixargzero", OF_EMULATE},
  {Option::Posixbuiltins, "posixbuiltins", OF_EMULATE | OF_BOURNE},
  {Option::Posixcd, "posixcd", OF_EMULATE | OF_BOURNE},
  {Option::Posixidentifiers, "posixidentifiers", OF_EMULATE | OF_BOURNE},
  {Option::Posixjobs, "posixjobs", OF_EMULATE | OF_BOURNE},
  {Option::Posixstrings, "posixstrings", OF_EMULATE | OF_BOURNE},
  {Option::Posixtraps, "posixtraps", OF_EMULATE | OF_BOURNE},
  {Option::Printeightbit, "printeightbit", 0},
  {Option::Printexitvalue, "printexitvalue", 0},
  {Option::Privileged, "privileged", OF_SPECIAL},
  {Option::Promptbang, "promptbang", OF_KSH},
  {Option::Promptcr, "promptcr", OF_ALL},
  {Option::Promptpercent, "promptpercent", OF_NON_BOURNE},
  {Option::Promptsp, "promptsp", OF_ALL},
  {Option::Promptsubst, "promptsubst", OF_BOURNE},
  {Option::Pushdignoredups, "pushdignoredups", OF_EMULATE},
  {Option::Pushdminus, "pushdminus", OF_EMULATE},
  {Option::Pushdsilent, "pushdsilent", 0},
  {Option::Pushdtohome, "pushdtohome", OF_EMULATE},
  {Option::Rcexpandparam, "rcexpandparam", OF_EMULATE},
  {Option::Rcquotes, "rcquotes", OF_EMULATE},
  {Option::Rcs, "rcs", OF_ALL},
  {Option::Recexact, "recexact", 0},
  {Option::Rematchpcre, "rematchpcre", 0},
  {Option::Rmstarsilent, "rmstarsilent", OF_BOURNE},
  {Option::Rmstarwait, "rmstarwait", 0},
  {Option::Sharehistory, "sharehistory", OF_KSH},
  {Option::Shfileexpansion, "shfileexpansion", OF_EMULATE | OF_BOURNE},
  {Option::Shglob, "shglob", OF_EMULATE | OF_BOURNE},
  {Option::Shinstdin, "shinstdin", OF_SPECIAL},
  {Option::Shnullcmd, "shnullcmd", OF_EMULATE | OF_BOURNE},
  {Option::Shoptionletters, "shoptionletters", OF_EMULATE | OF_BOURNE},
  {Option::Shortloops, "shortloops", OF_EMULATE | OF_NON_BOURNE},
  {Option::Shortrepeat, "shortrepeat", OF_EMULATE},
  {Option::Shwordsplit, "shwordsplit", OF_EMULATE | OF_BOURNE},
  {Option::Singlecommand, "singlecommand", OF_SPECIAL},
  {Option::Singlelinezle, "singlelinezle", OF_KSH},
  {Option::Sourcetrace, "sourcetrace", 0},
  {Option::Sunkeyboardhack, "sunkeyboardhack", 0},
  {Option::Transientrprompt, "transientrprompt", 0},
  {Option::Trapsasync, "trapsasync", 0},
  {Option::Typesetsilent, "typesetsilent", OF_EMULATE | OF_BOURNE},
  {Option::Typesettounset, "typesettounset", OF_EMULATE | OF_BOURNE},
  {Option::Unset, "unset", OF_EMULATE | OF_BSHELL},
  {Option::Verbose, "verbose", 0},
  {Option::Vi, "vi", 0},
  {Option::Warncreateglobal, "warncreateglobal", OF_EMULATE},
  {Option::Warnnestedvar, "warnnestedvar", OF_EMULATE},
  {Option::Xtrace, "xtrace", 0},
  {Option::Zle, "zle", OF_SPECIAL},
  {Option::Dvorak, "dvorak", 0},
});

struct OptionAlias {
  strv name;
  Option option;
  bool value;
};

constexpr auto OPTION_ALIASES = s::to_array<OptionAlias>({
  {"braceexpand", Option::Ignorebraces, false},
  {"dotglob", Option::Globdots, true},
  {"hashall", Option::Hashcmds, true},
  {"histappend", Option::Appendhistory, true},
  {"histexpand", Option::Banghist, true},
  {"log", Option::Histnofunctions, false},
  {"mailwarn", Option::Mailwarning, true},
  {"onecmd", Option::Singlecommand, true},
  {"physical", Option::Chaselinks, true},
  {"promptvars", Option::Promptsubst, true},
  {"stdin", Option::Shinstdin, true},
  {"trackall", Option::Hashcmds, true},
});

}  // namespace

s::span<const OptionInfo> option_table() { return OPTION_TABLE; }

strv option_name(Option option) {
  return OPTION_TABLE[static_cast<size_t>(option)].name;
}

opt<OptionLookup> lookup_option(strv name) {
  // TODO(impl): normalise (lowercase, drop '_'), try exact name, then
  // OPTION_ALIASES, then strip a leading "no" and retry with value=false.
  // "notify" must resolve to Notify, not to "no"+"tify".
  todo("lookup_option");
}

opt<OptionLookup> lookup_option_letter(char letter, Emulation emulation) {
  // TODO(impl): port zshletters[] / kshletters[] from Src/options.c, e.g.
  // zsh: 'e' ERREXIT, 'x' XTRACE, 'u' -UNSET, 'n' -EXEC, 'C' -CLOBBER.
  todo("lookup_option_letter");
}

bool option_default(Option option, Emulation emulation) {
  // TODO(impl): the emulation's OF_* bit in the table entry; also handle
  // runtime defaults (INTERACTIVE, SHINSTDIN, MONITOR) in Shell::init.
  todo("option_default");
}

bool OptionState::is_set(Option option) const {
  // TODO(impl)
  todo("OptionState::is_set");
}

bool OptionState::set(Option option, bool value) {
  // TODO(impl): refuse INTERACTIVE/SHINSTDIN/SINGLECOMMAND changes after
  // startup (see dosetopt()); return false in that case.
  todo("OptionState::set");
}

bool OptionState::set_by_name(strv name, bool value) {
  // TODO(impl): lookup_option then set; an unknown name returns false.
  todo("OptionState::set_by_name");
}

void OptionState::emulate(Emulation emulation, bool fully) {
  // TODO(impl): only OF_EMULATE options unless fully; never OF_SPECIAL ones.
  todo("OptionState::emulate");
}

void OptionState::reset(Emulation emulation) {
  // Constructors run this before any test can observe it, so it must not
  // throw. TODO(impl): set every option to option_default(option, emulation).
  mBits.reset();
  mEmulation = emulation;
}

str OptionState::dash_flags() const {
  // TODO(impl)
  todo("OptionState::dash_flags");
}

vec<str> OptionState::changed_options() const {
  // TODO(impl)
  todo("OptionState::changed_options");
}

}  // namespace myzsh
