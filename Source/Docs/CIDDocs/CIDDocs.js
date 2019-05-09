
//
//  We need the top level directory in a few places below, because we work
//  in terms of fully qualified URLs. We want to be flexible in where the
//  help is placed, so that we can post preview versions or old product
//  versions, so we get it from the initial incoming URL.
//
var helpRoot = null;


// -------------------------------------------------------------------
//  Handle initialization on the main page. We need to get the main
//  generated content page loaded. There is some basic text in it, but
//  we need to get the main (top level) topic list loaded into the left
//  hand side div.
// -------------------------------------------------------------------
function doInit()
{
    //
    //  We need the top level directory in a few places below, because we work
    //  in terms of fully qualified URLs. We want to be flexible in where the
    //  help is placed, so that we can post preview versions or old product
    //  versions, so we get it from the initial incoming URL.
    //
    helpRoot = window.location.href.split('?')[0];
    var to = helpRoot.lastIndexOf('/');
    to = to == -1 ? helpRoot.length : to + 1;
    helpRoot = helpRoot.substring(0, to - 1);

    // See if we have been asked to do debug mode
    debugCQSL = getParameterByName("debug");

   //
    //  Get the invocation URL. If there are no query parms for a page to load, we load
    //  the default initial page. Else we load the indicated one.
    //
    var tarTopic = getParameterByName("topic");
    var tarPage = getParameterByName("page");
    var tarBookMark = getParameterByName("bookmark");

    if (!tarTopic && !tarPage)
    {
        tarTopic = "/";
        tarPage = "/Overview";
    }
    if (!tarBookMark)
        tarBookMark = "";

    // Start us off with the initial location on the history stack
    if (history.replaceState) {

        var stateObj = {
            topic : tarTopic,
            page : tarPage,
            bookmark : tarBookMark
        };

        var initURL = helpRoot + "/CIDDocs.html?topic="
                      + tarTopic + "&page=" + tarPage;
        if (tarBookMark !== "")
            initURL += "&bookmark=" + tarBookMark;

        history.replaceState(stateObj, null, initURL);
    }

    // Load the initial left/right side content
    loadTopic(tarTopic, tarPage, false);

    //
    //  Set up a history state pop handler so that we can reload the right stuff when
    //  the user uses the back button.
    //
    window.onpopstate = function(event) {

        var newState = event.state;
        if (newState) {

            // It's a back operation, so go back
            var newTopic = newState["topic"];
            var newPage = newState["page"];
            var newBookMark = newState["bookmark"];

            // Load the restored left/right side URLs
            loadTopic(newTopic, newPage, newBookMark, false);

            // Show the new page path
            document.getElementById("CurHelpPath").innerText = makePagePath(newTopic, newPage);

            // If restoring a bookmark try to make it visible
            if (newBookMark !== "") {
                var bmElem = document.getElementById(newBookMark);
                if (bmElem)
                    bmElem.scrollIntoView();
            }
        }
    };
}


// -------------------------------------------------------------------
//  This is a helper that we can use to get to a particular query parameter on the URL that
//  we were invoked with.
//
//  Because the parameters we get are well defined and simple, we can use a simple method
//  to get them out. Basically we split once on &, and for each of those we split on =. None
//  of our values need to be decoded or have spaces in them. And since we only look at the
//  search part, the fact that they have forward slashes in them is not ambiguous. We never
//  see the main part of the URL.
// -------------------------------------------------------------------
function getParameterByName(name)
{
    // Get a copy of the query string
    var searchStr = window.location.search;

    // If it happens they don't support that, let's get out of the full URL
    if (!searchStr)
    {
        if (window.location.href)
        {
            var splitList = window.location.href.split('?');
            if (splitList && (splitList.length === 2))
            searchStr = splitList[1];
        }
    }

    // Optimize if no query string at all
    if (!searchStr || (searchStr.length === 0))
        return null;

    // Get rid of the leading ? if it is present
    if (searchStr[0] === '?')
        searchStr = searchStr.substr(1);

    // Split that now for our outer list
    var outerList = searchStr.split('&');
    var retVal = null;

    for (i = 0; i < outerList.length; i++)
    {
        var curParm = outerList[i].split('=');

        // If we got anything, and it's for the one we want
        if (curParm && (curParm[0].toUpperCase() === name.toUpperCase()))
        {
            // If we got a value, return that
            if (curParm.length > 1)
                retVal = curParm[1];

            // Break either way
            break;
        }
    }
    return retVal;
}



//
//  Called from topic index links that link to current level files. We just
//  load the indicated page into the right side DIV and update the bookmarks
//  if needed. We also update the current path display to reflect the new guy.
//
function loadRightSide(topicDir, pageToLoad, bookMark)
{
    // Set up an undo history state for thew new page
    pushPageHistory(topicDir, pageToLoad, bookMark);

    // Generate the page URL and load it to the right side DIV
    loadDIV(document.getElementById("RightSide"), makePageURL(topicDir, pageToLoad));

    // If we have a book mark, try to make it visible
    if (bookMark !== "") {
        var bmElem = document.getElementById(bookMark);
        if (bmElem)
            bmElem.scrollIntoView();
    }

    // Show the page path to the user
    document.getElementById("CurHelpPath").innerText = makePagePath(topicDir, pageToLoad);
    return true;
}


//
//  Called from topic index links that link to a sub-topic (or back up to the
//  parent topic.) We load the main topic page for that topic into the right side
//  div, and the topic index fro that topic into the left side div.
//
//  It's also called from a click on a generated page type link, which will pass in
//  the same info.
//
function loadTopic(topicDir, topicPage, pushHistory)
{
    // Push an undo history state for this new page
    if (pushHistory)
        pushPageHistory(topicDir, topicPage, "");

    // Create our left/right side URLs and load them to their respective DIVs
    var topicURL = makeTopicURL(topicDir);
    var pageURL = makePageURL(topicDir, topicPage);
    loadDIV(document.getElementById("LeftSide"), topicURL);
    loadDIV(document.getElementById("RightSide"), pageURL);

    // Show the page path to the user as well
    document.getElementById("CurHelpPath").innerText = makePagePath(topicDir, topicPage);

    return true;
}

async function loadDIV(tarDIV, toload) {
    tarDIV.innerHTML = await (await fetch(toload)).text();
}


//
//  Given a topic directory (which is the relative / style path to the topic)
//  generate a URL for that topic.
//
//  And the same for a right side (page) URL. And to create the 'page path' that
//  we display to the user.
//
function makeTopicURL(topicDir) {
    var topicURL = helpRoot + topicDir + "/CIDTopicIndex.html";
    topicURL = topicURL.replace(/\/\//g, '/');
    return topicURL;
}

function makePageURL(topicDir, pageName) {
    var pageURL = helpRoot + topicDir + "/" + pageName + ".html";
    pageURL = pageURL.replace(/\/\//g, '/');
    pageURL = pageURL.replace(/\/\//g, '/');
    return pageURL;
}

function makePagePath(topicDir, pageName) {
    var pagePath = topicDir + "/" + pageName;
    pagePath = pagePath.replace(/\/\//g, '/');
    pagePath = pagePath.replace(/\/\//g, '/');
    return pagePath;
}


// Pushes a topic onto the history stack
function pushPageHistory(topicDir, pageName, bookMark) {

    if (history.pushState) {
        var stateObj = {
            topic : topicDir,
            page : pageName,
            bookmark : bookMark
        };

        var newURL = helpRoot + "/CIDDocs.html?topic="
                     + topicDir + "&page=" + pageName;
        history.pushState(stateObj, "CID Topic", newURL);
    }
}


// -------------------------------------------------------------------
//  Kick off the initialization
// -------------------------------------------------------------------
window.onload=doInit;
