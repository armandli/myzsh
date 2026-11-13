#ifndef OPTIONS_H
#define OPTIONS_H

#include <bitset>
#include <cstdint>
#include <span>

#include <util/aliases.h>

namespace myzsh {

// One entry per zsh option, in the order of Src/options.c optns[].
enum class Option : int {
  Aliases,
  Aliasfuncdef,
  Allexport,
  Alwayslastprompt,
  Alwaystoend,
  Appendcreate,
  Appendhistory,
  Autocd,
  Autocontinue,
  Autolist,
  Automenu,
  Autonamedirs,
  Autoparamkeys,
  Autoparamslash,
  Autopushd,
  Autoremoveslash,
  Autoresume,
  Badpattern,
  Banghist,
  Bareglobqual,
  Bashautolist,
  Bashrematch,
  Beep,
  Bgnice,
  Braceccl,
  Bsdecho,
  Caseglob,
  Casematch,
  Casepaths,
  Cbases,
  Cprecedences,
  Cdablevars,
  Cdsilent,
  Chasedots,
  Chaselinks,
  Checkjobs,
  Checkrunningjobs,
  Clobber,
  Clobberempty,
  Combiningchars,
  Completealiases,
  Completeinword,
  Continueonerror,
  Correct,
  Correctall,
  Cshjunkiehistory,
  Cshjunkieloops,
  Cshjunkiequotes,
  Cshnullcmd,
  Cshnullglob,
  Debugbeforecmd,
  Emacs,
  Equals,
  Errexit,
  Errreturn,
  Exec,
  Extendedglob,
  Extendedhistory,
  Evallineno,
  Flowcontrol,
  Forcefloat,
  Functionargzero,
  Glob,
  Globalexport,
  Globalrcs,
  Globassign,
  Globcomplete,
  Globdots,
  Globstarshort,
  Globsubst,
  Hashcmds,
  Hashdirs,
  Hashexecutablesonly,
  Hashlistall,
  Histallowclobber,
  Histbeep,
  Histexpiredupsfirst,
  Histfcntllock,
  Histfindnodups,
  Histignorealldups,
  Histignoredups,
  Histignorespace,
  Histlexwords,
  Histnofunctions,
  Histnostore,
  Histsubstpattern,
  Histreduceblanks,
  Histsavebycopy,
  Histsavenodups,
  Histverify,
  Hup,
  Ignorebraces,
  Ignoreclosebraces,
  Ignoreeof,
  Incappendhistory,
  Incappendhistorytime,
  Interactive,
  Interactivecomments,
  Ksharrays,
  Kshautoload,
  Kshglob,
  Kshoptionprint,
  Kshtypeset,
  Kshzerosubscript,
  Listambiguous,
  Listbeep,
  Listpacked,
  Listrowsfirst,
  Listtypes,
  Localoptions,
  Localloops,
  Localpatterns,
  Localtraps,
  Login,
  Longlistjobs,
  Magicequalsubst,
  Mailwarning,
  Markdirs,
  Menucomplete,
  Monitor,
  Multifuncdef,
  Multios,
  Nomatch,
  Notify,
  Nullglob,
  Numericglobsort,
  Octalzeroes,
  Overstrike,
  Pathdirs,
  Pathscript,
  Pipefail,
  Posixaliases,
  Posixargzero,
  Posixbuiltins,
  Posixcd,
  Posixidentifiers,
  Posixjobs,
  Posixstrings,
  Posixtraps,
  Printeightbit,
  Printexitvalue,
  Privileged,
  Promptbang,
  Promptcr,
  Promptpercent,
  Promptsp,
  Promptsubst,
  Pushdignoredups,
  Pushdminus,
  Pushdsilent,
  Pushdtohome,
  Rcexpandparam,
  Rcquotes,
  Rcs,
  Recexact,
  Rematchpcre,
  Rmstarsilent,
  Rmstarwait,
  Sharehistory,
  Shfileexpansion,
  Shglob,
  Shinstdin,
  Shnullcmd,
  Shoptionletters,
  Shortloops,
  Shortrepeat,
  Shwordsplit,
  Singlecommand,
  Singlelinezle,
  Sourcetrace,
  Sunkeyboardhack,
  Transientrprompt,
  Trapsasync,
  Typesetsilent,
  Typesettounset,
  Unset,
  Verbose,
  Vi,
  Warncreateglobal,
  Warnnestedvar,
  Xtrace,
  Zle,
  Dvorak,
};

constexpr size_t OPTION_COUNT = static_cast<size_t>(Option::Dvorak) + 1;

enum class Emulation : int {
  Zsh,
  Sh,
  Ksh,
  Csh,
};

// Option flags from Src/options.c: which emulations turn the option on by
// default, whether `emulate` resets it, and whether it is special (can't be
// changed by the user or by emulate).
constexpr unsigned OF_CSH = 1u << 0;
constexpr unsigned OF_KSH = 1u << 1;
constexpr unsigned OF_SH = 1u << 2;
constexpr unsigned OF_ZSH = 1u << 3;
constexpr unsigned OF_ALL = OF_CSH | OF_KSH | OF_SH | OF_ZSH;
constexpr unsigned OF_BOURNE = OF_KSH | OF_SH;
constexpr unsigned OF_BSHELL = OF_KSH | OF_SH | OF_ZSH;
constexpr unsigned OF_NON_BOURNE = OF_ALL & ~OF_BOURNE;
constexpr unsigned OF_NON_ZSH = OF_ALL & ~OF_ZSH;
constexpr unsigned OF_EMULATE = 1u << 4;
constexpr unsigned OF_SPECIAL = 1u << 5;

struct OptionInfo {
  Option option;
  strv name;
  unsigned flags;
};

s::span<const OptionInfo> option_table();
strv option_name(Option option);

struct OptionLookup {
  Option option;
  // false for "noFOO" spellings and for letters that map to a negated option.
  bool value;
};

// Names are case-insensitive and ignore underscores ("No_Notify" works); a
// leading "no" negates; the bash/ksh aliases (braceexpand, dotglob, ...) are
// accepted.
opt<OptionLookup> lookup_option(strv name);
// Single-letter options as used by `set -X` and the command line; the letter
// set differs between zsh and sh/ksh emulation.
opt<OptionLookup> lookup_option_letter(char letter, Emulation emulation);

bool option_default(Option option, Emulation emulation);

struct OptionState {
  OptionState() { reset(Emulation::Zsh); }

  bool is_set(Option option) const;
  // Returns false when the option may not be changed (e.g. INTERACTIVE).
  bool set(Option option, bool value);
  bool set_by_name(strv name, bool value);

  // `emulate X`: reset emulation-relevant options; `emulate -R X` (fully)
  // resets every non-special option.
  void emulate(Emulation emulation, bool fully);
  void reset(Emulation emulation);
  Emulation emulation() const { return mEmulation; }

  // Value of $-: letters of every set option that has one.
  str dash_flags() const;
  // Output format of bare `setopt`: options differing from the emulation
  // default, as "name" or "noname", sorted.
  vec<str> changed_options() const;

private:
  s::bitset<OPTION_COUNT> mBits;
  Emulation mEmulation = Emulation::Zsh;
};

}  // namespace myzsh

#endif  // OPTIONS_H
