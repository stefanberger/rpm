/** \ingroup rpmbuild
 * \file build/names.c
 * Simple user/group name/id cache (plus hostname and buildtime)
 */


#include "system.h"

#include "rpmbuild.h"
#include "debug.h"

static uid_t uids[1024];
/*@owned@*/ /*@null@*/ static const char *unames[1024];
static int uid_used = 0;

static gid_t gids[1024];
/*@owned@*/ /*@null@*/ static const char *gnames[1024];
static int gid_used = 0;
    
void freeNames(void)
{
    int x;
    for (x = 0; x < uid_used; x++)
	free((void *)unames[x]);
    for (x = 0; x < gid_used; x++)
	free((void *)gnames[x]);
}

const char *getUname(uid_t uid)
{
    struct passwd *pw;
    int x;

    for (x = 0; x < uid_used; x++) {
	if (uids[x] == uid) {
	    return unames[x];
	}
    }

    /* XXX - This is the other hard coded limit */
    if (x == 1024)
	rpmlog(RPMLOG_CRIT, _("getUname: too many uid's\n"));
    
    pw = getpwuid(uid);
    uids[x] = uid;
    uid_used++;
    if (pw) {
	unames[x] = xstrdup(pw->pw_name);
    } else {
	unames[x] = NULL;
    }
    return unames[x];
}

const char *getUnameS(const char *uname)
{
    struct passwd *pw;
    int x;

    for (x = 0; x < uid_used; x++) {
	if (!strcmp(unames[x],uname)) {
	    return unames[x];
	}
    }

    /* XXX - This is the other hard coded limit */
    if (x == 1024)
	rpmlog(RPMLOG_CRIT, _("getUnameS: too many uid's\n"));
    
    pw = getpwnam(uname);
    uid_used++;
    if (pw) {
        uids[x] = pw->pw_uid;
	unames[x] = xstrdup(pw->pw_name);
    } else {
        uids[x] = -1;
	unames[x] = xstrdup(uname);
    }
    return unames[x];
}

const char *getGname(gid_t gid)
{
    struct group *gr;
    int x;

    for (x = 0; x < gid_used; x++) {
	if (gids[x] == gid) {
	    return gnames[x];
	}
    }

    /* XXX - This is the other hard coded limit */
    if (x == 1024)
	rpmlog(RPMLOG_CRIT, _("getGname: too many gid's\n"));
    
    gr = getgrgid(gid);
    gids[x] = gid;
    gid_used++;
    if (gr) {
	gnames[x] = xstrdup(gr->gr_name);
    } else {
	gnames[x] = NULL;
    }
    return gnames[x];
}

const char *getGnameS(const char *gname)
{
    struct group *gr;
    int x;

    for (x = 0; x < gid_used; x++) {
	if (!strcmp(gnames[x], gname)) {
	    return gnames[x];
	}
    }

    /* XXX - This is the other hard coded limit */
    if (x == 1024)
	rpmlog(RPMLOG_CRIT, _("getGnameS: too many gid's\n"));
    
    gr = getgrnam(gname);
    gid_used++;
    if (gr) {
    	gids[x] = gr->gr_gid;
	gnames[x] = xstrdup(gr->gr_name);
    } else {
    	gids[x] = -1;
	gnames[x] = xstrdup(gname);
    }
    return gnames[x];
}

time_t *const getBuildTime(void)
{
    static time_t buildTime = 0;

    if (! buildTime)
	buildTime = time(NULL);
    return &buildTime;
}

const char *const buildHost(void)
{
    static char hostname[1024];
    static int gotit = 0;
    struct hostent *hbn;

    if (! gotit) {
        gethostname(hostname, sizeof(hostname));
	if ((hbn = /*@-unrecog@*/ gethostbyname(hostname) /*@=unrecog@*/ )) {
	    strcpy(hostname, hbn->h_name);
	} else {
	    rpmMessage(RPMMESS_WARNING, _("Could not canonicalize hostname: %s\n"),
		    hostname);
	}
	gotit = 1;
    }
    return(hostname);
}
